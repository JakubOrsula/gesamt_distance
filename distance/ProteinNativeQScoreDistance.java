/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package messif.distance.impl;

import messif.distance.DistanceFunc;
import messif.distance.Metric;

/**
 *
 * @author xmic
 */
public class ProteinNativeQScoreDistance implements DistanceFunc<String>, Metric {

    /**
     * Class id for Java serialization.
     */
    private static final long serialVersionUID = 124687651462L;

    /**
     * Threshold in seconds for each distance evaluation. If evaluation
     * shouldtake more time, it is interrupted and maximum disatcne is returned.
     */
    private final float timeThresholdForEval;

    private static native void init(String archiveDirectory, String preloadList, boolean binaryArchive, double inherentApprox);

    private native float getNativeDistance(String id1, String id2, float timeThresholdInSeconds);

    public ProteinNativeQScoreDistance(float timeThresholdForEval) {
        this.timeThresholdForEval = timeThresholdForEval;
    }

    /**
     * Must be called before first evaluation. Objects from specified file are
     * read into the main memorys for more efficient distance evaluations
     *
     * @param jsonPivotsPath
     */
    public static void initDistance(String jsonPivotsPath) {
        System.loadLibrary("ProteinDistance");
        init("/mnt/data/PDBe_clone_binary", jsonPivotsPath, true, 0.6);
    }

    @Override
    public float getDistance(String o1, String o2, float threshold) {
        return getNativeDistance(o1, o2, timeThresholdForEval);
    }

    @Override
    public Class<String> getObjectClass() {
        return String.class;
    }

    @Override
    public String toString() {
        return getClass().getSimpleName();
    }

    @Override
    public float getMaxDistance() {
        return 1f;
    }

}
