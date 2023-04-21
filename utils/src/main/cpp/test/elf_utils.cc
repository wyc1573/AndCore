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

struct FilterSectionParam {
    char* elf_start;
    const char* section_name;
    void* section_hdr;
    bool elf_64;
};

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
        LOG(INFO) << "ELF64";
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


        for (int i = 0; i < shnum; i++) {
            Elf64_Shdr* tshdr = reinterpret_cast<Elf64_Shdr*>(shdr + i);
            FilterSectionParam param {elf_start, sh_strtab_p + tshdr->sh_name, reinterpret_cast<void*>(tshdr), true};
            if (visitor(param)) {
                LOG(INFO) << "exit Loop Section Header.";
                break;
            }
        }

        LOG(INFO) << "program header";
        Elf64_Phdr * phdr = reinterpret_cast<Elf64_Phdr*>(elf_start + ehdr->e_phoff);
        for (int i = 0; i < ehdr->e_phnum; i++) {
            phdr = phdr + i;
            LOG(INFO) << i;
            LOG(INFO) << "\nphdr->p_offset: " << StringPrintf("0x%x", phdr->p_offset)
            << "\nphdr->p_filesz: " << StringPrintf("0x%x", phdr->p_filesz)
            << "\nphdr->p_flags: " << StringPrintf("0x%x", phdr->p_flags)
            << "\nphdr->p_memsz: " << StringPrintf("0x%x", phdr->p_memsz)
            << "\nphdr->p_paddr: " << StringPrintf("0x%x", phdr->p_paddr)
            << "\nphdr->p_type: " << pt_type(phdr->p_type)
            << "\nphdr->p_vaddr: " << StringPrintf("0x%x", phdr->p_vaddr)
            << "\n";

        }

    } else {
        LOG(INFO) << "ELF32";
        Elf32_Ehdr* ehdr = reinterpret_cast<Elf32_Ehdr*>(elf_start);
        Elf32_Shdr* shdr = reinterpret_cast<Elf32_Shdr*>(elf_start + ehdr->e_shoff);
        Elf32_Half shnum = ehdr->e_shnum;
        Elf32_Shdr* sh_strtab = shdr + ehdr->e_shstrndx;
        const char *const sh_strtab_p = reinterpret_cast<char*>(elf_start + sh_strtab->sh_offset);

        LOG(INFO) << "\nheader->e_type: " << ehdr->e_type
                  << "\n header->e_machine: " << ehdr->e_machine
                  << "\n header-> e_version: " << ehdr->e_version
                  << "\n header->e_shoff: " << StringPrintf("0x%x", ehdr->e_shoff)
                  << "\n header->e_flags" << StringPrintf("0x%x", ehdr->e_flags)
                  << "\n header->e_entry: " << StringPrintf("0x%x", ehdr->e_entry)
                  << "\n header->e_shentsize: " << ehdr->e_shentsize
                  << "\n header->e_shnum: " << ehdr->e_shnum
                  << "\n header->e_shstrndx: " << ehdr->e_shstrndx
                  << "\n header->e_phnum: " << ehdr->e_phnum
                  << "\n header->e_phoff: " << StringPrintf("0x%x", ehdr->e_phoff)
                  << "\n"
                ;

        for (int i = 0; i < shnum; i++) {
            Elf32_Shdr* tshdr = reinterpret_cast<Elf32_Shdr*>(shdr + i);
            FilterSectionParam param {elf_start, sh_strtab_p + tshdr->sh_name, reinterpret_cast<void*>(tshdr), false};
            if (visitor(param)) {
                LOG(INFO) << "exit Loop Section Header.";
                break;
            }
        }

        LOG(INFO) << "program header";
        Elf32_Phdr * phdr = reinterpret_cast<Elf32_Phdr*>(elf_start + ehdr->e_phoff);
        for (int i = 0; i < ehdr->e_phnum; i++) {
            phdr = phdr + i;
            LOG(INFO) << i;
            LOG(INFO) << "\nphdr->p_offset: " << StringPrintf("0x%x", phdr->p_offset)
                      << "\nphdr->p_filesz: " << StringPrintf("0x%x", phdr->p_filesz)
                      << "\nphdr->p_flags: " << StringPrintf("0x%x", phdr->p_flags)
                      << "\nphdr->p_memsz: " << StringPrintf("0x%x", phdr->p_memsz)
                      << "\nphdr->p_paddr: " << StringPrintf("0x%x", phdr->p_paddr)
                      << "\nphdr->p_type: " << pt_type(phdr->p_type)
                      << "\nphdr->p_vaddr: " << StringPrintf("0x%x", phdr->p_vaddr)
                      << "\n";
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


void test_elf() {

    std::string error_msg;
    std::string path;
    GetPathFromMaps("/libUtils.so", path, error_msg);
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

    auto section_visitor = [&] (FilterSectionParam param) -> bool {
        if (strcmp(".note.gnu.build-id", param.section_name) == 0) {
            LOG(INFO) << "Find Build Id Section";
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
            } else {
                Elf32_Shdr* shdr = reinterpret_cast<Elf32_Shdr*>(param.section_hdr);
                Elf32_Off offset = shdr->sh_offset;
                Elf32_Word size = shdr->sh_size;
                int8_t* build_id_addr = reinterpret_cast<int8_t*>(param.elf_start + offset);
                Elf32_Nhdr* build_id_note_hdr = reinterpret_cast<Elf32_Nhdr*>(build_id_addr);
                Elf32_Word descsz = build_id_note_hdr->n_descsz;
                char* build_id = (char*) calloc(1, descsz * 2 + 1);
                for (int i = 0; i < size; i++) {
                    sprintf(build_id, "%s%02hhx", build_id, *(build_id_addr + i));
                }
                LOG(INFO) << StringPrintf("BuildId(%s)", build_id);
            }
            return false;
        } else if (strcmp(".dynsym", param.section_name) == 0) {
            LOG(INFO) << param.section_name;
            if (param.elf_64) {
                Elf64_Shdr* shdr = reinterpret_cast<Elf64_Shdr*>(param.section_hdr);
                Elf64_Off offset = shdr->sh_offset;
                Elf64_Xword sh_size = shdr->sh_size;
                Elf64_Xword entsize = shdr->sh_entsize;
                Elf64_Sym* sym = reinterpret_cast<Elf64_Sym *>(param.elf_start + offset);
                int nsym = sh_size / entsize;
                for (int i = 0; i < nsym; i++) {
                    sym += i;
                    LOG(INFO) << i << ": st_name = " << (str_tab + sym->st_name)
                    << ",\n     st_size = " << sym->st_size
                    << ", st_value = " << sym->st_value
                    << ", st_shndx = " << sym->st_shndx
                    << ", STB = " << STB(ELF_ST_BIND(sym->st_info))
                    << ", STT = " << STT(ELF_ST_TYPE(sym->st_info))
                    << ", sym->st_other = " << (int) sym->st_other;
                }
            }
        }
        return false;
    };

    WalkElfSections(path.c_str(), section_visitor);
}
