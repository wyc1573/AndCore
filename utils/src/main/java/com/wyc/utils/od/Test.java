package com.wyc.utils.od;

import android.media.MediaCodec;

import java.util.Scanner;
import java.util.regex.Pattern;

/**
 * <description>
 *
 * @author wyc
 * @date 2023/9/15
 */
public class Test {
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
//        String line = in.nextLine();
        String line = "AAAAA333";
        if(line.matches("A*3*")) {
            System.out.println("match");
        }
        /*
        String[] arr = line.split(" ");
        String text = arr[0];
        String pattern = arr[1];
        int idx = pattern.indexOf('*');
        String pLeft = pattern.substring(0, idx);
        String pRight = pattern.substring(idx+1);
        int cnt = 0;

        for(int i = 0; i < text.length()-1; i++) {
            int leftLen = pLeft.length();
            int rLen = pRight.length();
            if (i + leftLen > text.length()) continue;
            String left = text.substring(i, i + leftLen);


            if (left.equals(pLeft)) {
                for (int j = i + leftLen; j < text.length(); j++) {
                    if (j + rLen > text.length()) continue;
                    String rStr =text.substring(j, j+rLen);
                    if (rStr.equals(pRight)) {
                        if (text.indexOf(pLeft, j) < 0) {
                            if(text.indexOf(pRight) >= j) {
                                cnt++;
                                break;
                            }
                        }
                    }
                }
            }
        }

        System.out.println(cnt);*/
    }


}
