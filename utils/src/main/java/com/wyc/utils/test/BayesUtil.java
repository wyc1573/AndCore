package com.wyc.utils.test;

import android.os.SystemClock;
import android.util.Log;


import java.math.BigDecimal;
import java.util.Random;

public class BayesUtil {
    public static final int SAMPLE_NUM = 1000000;
    public static final int FEATURE_NUM = 5;

    public static final int BUG_FEATURE_INDEX = 3;

    public static final int CRASH_PERCENT = 1;

    public static final int[] FEATURE_PERCENT = new int[FEATURE_NUM];

    public static int[] featureCrashCount = new int[FEATURE_NUM];

    public static int[] featureCount = new int[FEATURE_NUM];

    private static double div(double d1, double d2, int scale) {
        BigDecimal b1 = new BigDecimal(Double.toString(d1));
        BigDecimal b2 = new BigDecimal(Double.toString(d2));
        return b1.divide(b2, scale, BigDecimal.ROUND_HALF_UP).doubleValue();
    }

    private static void initFeaturePercent() {
        Random rand = new Random(System.currentTimeMillis());
        for (int i = 0; i < FEATURE_NUM; i++) {
            FEATURE_PERCENT[i] = rand.nextInt(100);
        }
    }

    private static void printStats() {
        StringBuilder sb = new StringBuilder();
        sb.append("FEATURE_PERCENT: { ");
        for (int i = 0; i < FEATURE_NUM; i++) {
            sb.append("[").append(i).append("]")
                    .append(" = ").append(FEATURE_PERCENT[i]).append("%");
            if (i != FEATURE_NUM - 1) {
                sb.append(", ");
            }
        }
        sb.append("}");
        Log.d("AndCore", sb.toString());
        StringBuilder sb2 = new StringBuilder("[featureCrashCount, featureCount] = {");
        for (int i = 0; i < FEATURE_NUM; i++) {
            sb2.append('[').append(featureCrashCount[i]).append(", ")
                    .append(featureCount[i]).append(']');
            if (i != FEATURE_NUM - 1) {
                sb2.append(", ");
            }
        }
        sb2.append("}");
        Log.d("AndCore", sb2.toString());
        StringBuilder sb3 = new StringBuilder("Probability: [");
        for (int i = 0; i < FEATURE_NUM; i++) {
            sb3.append(div(featureCrashCount[i], featureCount[i], 2) * 100).append("%");
            if (i != FEATURE_NUM - 1) {
                sb3.append(", ");
            }
        }
        sb3.append(']');
        Log.d("AndCore", sb3.toString());
    }


    private static void startMock() {
        Random crashRandom = new Random(System.currentTimeMillis());
        Random featureRandom = new Random(SystemClock.uptimeMillis());
        initFeaturePercent();
        for (int i = 0; i < SAMPLE_NUM; i++) {
            boolean crashHappen = crashRandom.nextInt(100) < CRASH_PERCENT;
            for (int j = 0; j < FEATURE_NUM; j++) {
                boolean featureHit = featureRandom.nextInt(100) < FEATURE_PERCENT[j];
                if (featureHit) {
                    featureCount[j]++;
                    if (j == BUG_FEATURE_INDEX) {
                        featureCrashCount[j]++;
                    } else if (featureHit){
                        featureCrashCount[j] = crashHappen ? featureCrashCount[j] + 1 : featureCrashCount[j];
                    }
                }
            }
        }
        printStats();
    }

    public static void mock() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                startMock();
            }
        }).start();
    }

}
