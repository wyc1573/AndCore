//
// Created by wyc on 2023/4/4.
//

#include "p_elf.h"
#include <vector>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>
#include "android-base/file.h"
#include "android-base/unique_fd.h"
#include "android-base/logging.h"
#include "android-base/stringprintf.h"
#include "env.h"

using namespace android::base;

static int sh_idx_ = 0;

struct FilterSectionParam {
    char* elf_start;
    const char* section_name;
    void* section_hdr;
    bool elf_64;
};

const char* SH_TYPE(int64_t type) {
    switch (type) {
        case 0:
            return "SHT_NULL";
        case 1:
            return "SHT_PROGBITS";
        case 2:
            return "SHT_SYMTAB";
        case 3:
            return "SHT_STRTAB";
        case 4:
            return "SHT_RELA";
        case 5:
            return "SHT_HASH";
        case 6:
            return "SHT_DYNAMIC";
        case 7:
            return "SHT_NOTE";
        case 8:
            return "SHT_NOBITS";
        case 9:
            return "SHT_REL";
        case 10:
            return "SHT_SHLIB";
        case 11:
            return "SHT_DYNSYM";
        case 12:
            return "SHT_NUM";
        case 0x70000000:
            return "SHT_LOPROC";
        case 0x7fffffff:
            return "SHT_HIPROC";
        case 0x80000000:
            return "SHT_LOUSER";
        case 0xffffffff:
            return "SHT_HIUSER";
    }
    return "UNKNOWN";
}

const char* SH_FLAGS(int64_t flags) {
    switch (flags) {
        case 0x1:
            return "SHF_WRITE";
        case 0x2:
            return "SHF_ALLOC";
        case 0x4:
            return "SHF_EXECINSTR";
        case 0x00100000:
            return "SHF_RELA_LIVEPATCH";
        case 0x00200000:
            return "SHF_RO_AFTER_INIT";
        case 0xf0000000:
            return "SHF_MASKPROC";
    }
    return "UnKnown";
}

void PrintSHdr(FilterSectionParam& param) {
    Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
    LOG(INFO) << sh_idx_++ << ".\t Section Header { NAME: " << param.section_name
    << ", OFFSET: " << shdr->sh_offset << StringPrintf("(%p)", shdr->sh_offset)
    << ", TYPE: " << SH_TYPE(shdr->sh_type)
    << ", FLAGS: " << SH_FLAGS(shdr->sh_flags)
    << ", LINK: " << shdr->sh_link
    << ", ADDR: " << StringPrintf("%p", shdr->sh_addr)
    << ", SIZE: " << shdr->sh_size << StringPrintf("(%p)", shdr->sh_size)
    << " }";
}

bool GetPathFromMaps(const char* const path, std::string& res, std::string& error_msg) {
    if (path == nullptr || path[0] != '/') {
        LOG(INFO) << "illegal param, return!";
        return false;
    }
    std::string err_msg;
    unique_fd proc_self_maps(open("/proc/self/maps",  O_RDONLY));
    auto filter = [&](const char* const line)-> bool {
        if (strstr(line, path)) {
            const char* s = line;
            while (*s != '/') {
                s++;
            }
            res.assign(s);
            return true;
        }
        return false;
    };
    bool ret = android::base::ForEachLine(filter, proc_self_maps.release(), err_msg);
    return ret;
}

bool GetPathsFromMaps(std::unordered_map<std::string, std::string>& paths_map, std::string& error_msg) {
    std::string err_msg;
    unique_fd proc_self_maps(open("/proc/self/maps",  O_RDONLY));
    auto filter = [&](const char* const line)-> bool {
        for (auto pair : paths_map) {
            if (strstr(line, pair.first.c_str())) {
                const char* s = line;
                while (*s != '/') {
                    s++;
                }
                pair.second.assign(s);
            }
        }
        return false;
    };
    bool ret = android::base::ForEachLine(filter, proc_self_maps.release(), err_msg);
    if (!ret) {
        LOG(INFO) << "[GetPathsFromMaps]: ForEachLine " << err_msg;
    }
    return ret;
}

bool IsELF64(char* elf_begin) {
    if (elf_begin[4] == ELFCLASS64) {
      return true;
    }

    return false;
}

const char* pt_type(Elf64_Word type) {
    switch (type) {
        case PT_NULL:
            return "PT_NULL";
        case PT_LOAD:
            return "PT_LOAD";
        case PT_DYNAMIC:
            return "PT_DYNAMIC";
        case PT_INTERP:
            return "PT_INTERP";
        case PT_NOTE:
            return "PT_NOTE";
        case PT_SHLIB:
            return "PT_SHLIB";
        case PT_PHDR:
            return "PT_PHDR";
        case PT_TLS:
            return "PT_TLS";
        case PT_LOOS:
            return "PT_LOOS";
        case PT_HIOS:
            return "PT_HIOS";
        case PT_LOPROC:
            return "PT_LOPROC";
        case PT_HIPROC:
            return "PT_HIPROC";
        case PT_GNU_EH_FRAME:
            return "PT_GNU_EH_FRAME";
        case PT_GNU_STACK:
            return "PT_GNU_STACK";
        case PT_GNU_RELRO:
            return "PT_GNU_RELRO";
        case PT_GNU_PROPERTY:
            return "PT_GNU_PROPERTY";
        default:
            return "UnKnown";
    }
}

void WalkElfSections(const char* const elf, std::function<bool(FilterSectionParam)> visitor) {
    int fd = open(elf, O_RDONLY);
    if (fd < 0) {
        LOG(INFO) << "[FilterElfSections]: open failed! return.";
        return;
    }

    struct stat stat_buf;
    fstat(fd, &stat_buf);

    char* elf_start = (char*) mmap(nullptr, stat_buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (elf_start == nullptr) {
        LOG(INFO) << "[FilterElfSections]: mmap failed. return!";
        return;
    }
    if (IsELF64(elf_start)) {
        LOG(INFO) << "ELF Header {";
        Elf64_Ehdr* ehdr = reinterpret_cast<Elf64_Ehdr*>(elf_start);

        Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(elf_start + ehdr->e_shoff);
        Elf64_Half shnum = ehdr->e_shnum;
        Elf64_Shdr* sh_strtab = shdr + ehdr->e_shstrndx;
        const char *const sh_strtab_p = reinterpret_cast<char*>(elf_start + sh_strtab->sh_offset);

        LOG(INFO) << " header->e_type: " << ehdr->e_type
                  << "\n header->e_machine: " << ehdr->e_machine
                  << "\n header-> e_version: " << ehdr->e_version
                  << "\n header->e_shoff: " << StringPrintf("0x%x", ehdr->e_shoff)
                  << "\n header->e_flags: " << StringPrintf("%x", ehdr->e_flags)
                  << "\n header->e_entry: " << StringPrintf("%x", ehdr->e_entry)
                  << "\n header->e_shentsize: " << ehdr->e_shentsize
                  << "\n header->e_shnum: " << ehdr->e_shnum
                  << "\n header->e_shstrndx: " << ehdr->e_shstrndx
                  << "\n header->e_phnum: " << ehdr->e_phnum
                  << "\n header->e_phoff: " << StringPrintf("0x%x", ehdr->e_phoff)
                ;
        LOG(INFO) << "}";
        LOG(INFO) << "Program Header {";
        Elf64_Phdr * phdr = reinterpret_cast<Elf64_Phdr*>(elf_start + ehdr->e_phoff);
        for (int i = 0; i < ehdr->e_phnum; i++) {
            phdr = phdr + i;
            LOG(INFO) << i << ":\n"
                      << "\nphdr->p_offset: " << StringPrintf("0x%x", phdr->p_offset)
                      << "\nphdr->p_filesz: " << StringPrintf("0x%x", phdr->p_filesz)
                      << "\nphdr->p_flags: " << StringPrintf("0x%x", phdr->p_flags)
                      << "\nphdr->p_memsz: " << StringPrintf("0x%x", phdr->p_memsz)
                      << "\nphdr->p_paddr: " << StringPrintf("0x%x", phdr->p_paddr)
                      << "\nphdr->p_type: " << pt_type(phdr->p_type)
                      << "\nphdr->p_vaddr: " << StringPrintf("0x%x", phdr->p_vaddr)
                      << "\n";
        }
        LOG(INFO) << "}";

        for (int i = 0; i < shnum; i++) {
            Elf64_Shdr* tshdr = reinterpret_cast<Elf64_Shdr*>(shdr + i);
            FilterSectionParam param {elf_start, sh_strtab_p + tshdr->sh_name, reinterpret_cast<void*>(tshdr), true};
            if (visitor(param)) {
                break;
            }
        }
    }
}

const char* STT(uint8_t type) {
    switch (type) {
        case 0:
            return "NOTYPE";
        case 1:
            return "OBJECT";
        case 2:
            return "FUNC";
        case 3:
            return "SECTION";
        case 4:
            return "FILE";
        case 5:
            return "COMMON";
        case 6:
            return "TLS";
    }
}

const char* STB(uint8_t type) {
    if (type == 0) {
        return "LOCAL";
    }
    if (type == 1) {
        return "GLOBAL";
    }
    if(type == 2) {
        return "WEAK";
    }
    return "UNKNOWN";
}

const char* DT_TAG(int32_t tag);

void test_elf() {

    std::string error_msg;
    std::string path;
    GetPathFromMaps("/libElfDemo.so", path, error_msg);
    const char* str_tab = nullptr;

    auto str_visitor = [&] (FilterSectionParam param)-> bool {
        if (strcmp(".dynstr", param.section_name) == 0) {
            LOG(INFO) << ".dynstr";
            Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
            Elf64_Off offset = shdr->sh_offset;
            str_tab = reinterpret_cast<const char *>(param.elf_start + offset);
            return true;
        }
        return false;
    };
    WalkElfSections(path.c_str(), str_visitor);


    LOG(INFO) << "Section Header";
    auto section_print_visitor = [&](FilterSectionParam param)->bool {
        PrintSHdr(param);
        return false;
    };
    WalkElfSections(path.c_str(), section_print_visitor);


    auto section_visitor = [&] (FilterSectionParam param) -> bool {
        if (strcmp(".note.gnu.build-id", param.section_name) == 0) {
            LOG(INFO) << ".note.gnu.build-id {";
            if (param.elf_64) {
                Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
                Elf64_Off offset = shdr->sh_offset;
                Elf64_Xword sh_size = shdr->sh_size;
                int8_t* build_id_addr = reinterpret_cast<int8_t*>(param.elf_start + offset);
                Elf64_Nhdr* build_id_note_hdr = reinterpret_cast<Elf64_Nhdr*>(build_id_addr);
                Elf64_Word descsz = build_id_note_hdr->n_descsz;

                char* build_id = (char*) calloc(1, descsz * 2 + 1);
                for (int i = sh_size - descsz; i < sh_size; i++) {
                    sprintf(build_id, "%s%02hhx", build_id, *(build_id_addr + i));
                }
                LOG(INFO) << StringPrintf("BuildId(%s)", build_id);
            }
            LOG(INFO) << "}";
            return false;
        } else if (strcmp(".dynsym", param.section_name) == 0) {
            LOG(INFO) << ".dynsym {";
            if (param.elf_64) {
                Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
                Elf64_Off offset = shdr->sh_offset;
                Elf64_Xword sh_size = shdr->sh_size;
                Elf64_Xword entsize = shdr->sh_entsize;
                Elf64_Sym* sym = reinterpret_cast<Elf64_Sym *>(param.elf_start + offset);
                int nsym = sh_size / entsize;
                LOG(INFO) << "entsize = " << entsize;
                for (int i = 0; i < nsym; i++) {
                    if (i == 500) {
                        LOG(INFO) << "more ... Total: " << nsym;
                        break;
                    }
                    LOG(INFO) << i << ":\t st_name = " << (str_tab + sym->st_name)
                    << ", st_size = " << sym->st_size
                    << ", st_value = " << sym->st_value
                    << ", st_shndx = " << sym->st_shndx
                    << ", STB = " << STB(ELF_ST_BIND(sym->st_info))
                    << ", STT = " << STT(ELF_ST_TYPE(sym->st_info))
                    << ", sym->st_other = " << (int) sym->st_other;
                    sym++;
                }
            }
            LOG(INFO) << "}";
        } else if (strcmp(".rela.dyn", param.section_name) == 0) {
            LOG(INFO) << ".rela.dyn Section {";
            if (param.elf_64) {
                Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
                Elf64_Off offset = shdr->sh_offset;
                Elf64_Xword sh_size = shdr->sh_size;
                Elf64_Xword entsize = shdr->sh_entsize;
                int num = sh_size / entsize;

                Elf64_Rela* rela = reinterpret_cast<Elf64_Rela*>(param.elf_start + offset);
                for (int i = 0; i < num; i++) {
                    LOG(INFO) << i << ": .rela.dyn r_offset: "<< rela->r_offset
                    << ", r_addend: " << rela->r_addend
                    << ", r_info = " << rela->r_info
                    << ", ELF64_R_SYM: " << ELF64_R_SYM(rela->r_info)
                    << ", ELF64_R_TYPE: " << ELF64_R_TYPE(rela->r_info);
                    if (i ==  500) {
                        LOG(INFO) << "more ... Total: " << num;
                        break;
                    }
                    rela++;
                }
            }
            LOG(INFO) << "}";
        } else if (strcmp(".dynamic", param.section_name) == 0) {
            LOG(INFO) << ".dynamic {";
            Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
            Elf64_Off offset = shdr->sh_offset;
            Elf64_Xword sh_size = shdr->sh_size;
            Elf64_Xword entsize = shdr->sh_entsize;
            int num = sh_size / entsize;

            Elf64_Dyn* dyn = reinterpret_cast<Elf64_Dyn *>(param.elf_start + offset);
            while (dyn->d_tag != NULL) {
                const char* tag = DT_TAG(dyn->d_tag);
                if (strcmp(tag, "DT_NEEDED") == 0) {
                    LOG(INFO) << "DT_NEEDED: " <<  (str_tab + dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_SONAME") == 0) {
                    LOG(INFO) << "DT_SONAME: " <<  (str_tab + dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_STRTAB") == 0) {
                    LOG(INFO) << "DT_STRTAB: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_SYMTAB") == 0) {
                    LOG(INFO) << "DT_SYMTAB: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_STRSZ") == 0) {
                    LOG(INFO) << "DT_STRSZ: " << dyn->d_un.d_val << StringPrintf("(%p)", dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_PLTGOT") == 0) {
                    LOG(INFO) << "DT_PLTGOT: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_PLTRELSZ") == 0) {
                    LOG(INFO) << "DT_PLTRELSZ: " << dyn->d_un.d_val << StringPrintf("(%p)", dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_PLTREL") == 0) {
                    LOG(INFO) << "DT_PLTREL: " << dyn->d_un.d_val << StringPrintf("(%p)", dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_INIT") == 0) {
                    LOG(INFO) << "DT_INIT: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_FINI") == 0) {
                    LOG(INFO) << "DT_FINI: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_INIT_ARRAY") == 0) {
                    LOG(INFO) << "DT_INIT_ARRAY: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_INIT_ARRAYSZ") == 0) {
                    LOG(INFO) << "DT_INIT_ARRAYSZ: " << dyn->d_un.d_val << StringPrintf("(%p)", dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_FINI_ARRAY") == 0) {
                    LOG(INFO) << "DT_FINI_ARRAY: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_FINI_ARRAYSZ") == 0) {
                    LOG(INFO) << "DT_FINI_ARRAYSZ: " << dyn->d_un.d_val << StringPrintf("(%p)", dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_GNU_HASH") == 0) {
                    LOG(INFO) << "DT_GNU_HASH: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_RELA") == 0) {
                    LOG(INFO) << "DT_RELA: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_RELASZ") == 0) {
                    LOG(INFO) << "DT_RELASZ: " << dyn->d_un.d_val << StringPrintf("(%p)", dyn->d_un.d_val);
                } else if (strcmp(tag, "DT_RELAENT") == 0) {
                    LOG(INFO) << "DT_RELAENT: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else if (strcmp(tag, "DT_NULL") == 0) {
                    LOG(INFO) << "DT_NULL: " << dyn->d_un.d_ptr << StringPrintf("(%p)", dyn->d_un.d_ptr);
                } else {
                    LOG(INFO) << "d_tag: " << tag;
                }
                dyn++;
            }
            LOG(INFO) << "}";
        } else if (strcmp(".gnu.version", param.section_name) == 0) {
            LOG(INFO) << ".gnu.version {";
            Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
            Elf64_Off offset = shdr->sh_offset;
            Elf64_Xword sh_size = shdr->sh_size;
            Elf64_Xword entsize = shdr->sh_entsize;
            int num = sh_size / entsize;
            for (int i = 0; i < num; i++) {
                LOG(INFO) << i << ". " << (str_tab);
            }
            LOG(INFO) << "}";
        } else if (strcmp(".plt", param.section_name) == 0) {
            LOG(INFO) << ".plt {";
            Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
            Elf64_Off offset = shdr->sh_offset;
            Elf64_Xword sh_size = shdr->sh_size;
            Elf64_Xword entsize = shdr->sh_entsize;
            int num = sh_size / entsize;
            LOG(INFO) << "\tnum: "  << num;
            LOG(INFO) << "}";
        }
        return false;
    };

    WalkElfSections(path.c_str(), section_visitor);
}


const char* DT_TAG(int32_t tag) {
    switch (tag) {
        case 0:
            return "DT_NULL";
        case 1:
            return "DT_NEEDED";
        case 2:
            return "DT_PLTRELSZ";
        case 3:
            return "DT_PLTGOT";
        case 4:
            return "DT_HASH";
        case 5:
            return "DT_STRTAB";
        case 6:
            return "DT_SYMTAB";
        case 7:
            return "DT_RELA";
        case 8:
            return "DT_RELASZ";
        case 9:
            return "DT_RELAENT";
        case 10:
            return "DT_STRSZ";
        case 11:
            return "DT_SYMENT";
        case 12:
            return "DT_INIT";
        case 13:
            return "DT_FINI";
        case 14:
            return "DT_SONAME";
        case 15:
            return "DT_RPATH";
        case 16:
            return "DT_SYMBOLIC";
        case 17:
            return "DT_REL";
        case 18:
            return "DT_RELSZ";
        case 19:
            return "DT_RELENT";
        case 20:
            return "DT_PLTREL";
        case 21:
            return "DT_DEBUG";
        case 22:
            return "DT_TEXTREL";
        case 23:
            return "DT_JMPREL";
        case 32:
            return "DT_ENCODING";
        case 0x60000000:
            return "OLD_DT_LOOS";
        case 0x6000000d:
            return "DT_LOOS";
        case 0x6ffff000:
            return "DT_HIOS";
        case 0x6ffffd00:
            return "DT_VALRNGLO";
        case 0x6ffffdff:
            return "DT_VALRNGHI";
        case 0x6ffffe00:
            return "DT_ADDRRNGLO";
        case 0x6ffffeff:
            return "DT_ADDRRNGHI";
        case 0x6ffffff0:
            return "DT_VERSYM";
        case 0x6ffffff9:
            return "DT_RELACOUNT";
        case 0x6ffffffa:
            return "DT_RELCOUNT";
        case 0x6ffffffb:
            return "DT_FLAGS_1";
        case 0x6ffffffc:
            return "DT_VERDEF";
        case 0x6ffffffd:
            return "DT_VERDEFNUM";
        case 0x6ffffffe:
            return "DT_VERNEED";
        case 0x6fffffff:
            return "DT_VERNEEDNUM";
        case 0x70000000:
            return "DT_LOPROC";
        case 0x7fffffff:
            return "DT_HIPROC";
        default:
            return "UNKNOWN";
    }
}