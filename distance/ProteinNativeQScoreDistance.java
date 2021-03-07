/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package messif.distance.impl;

import java.util.logging.Level;
import java.util.logging.Logger;
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

    public static final Float IMPLICIT_INNER_PARAMETER_ON_SIZE_CHECK = 0.6f;
    private static final Logger LOG = Logger.getLogger(ProteinNativeQScoreDistance.class.getName());

    /**
     * Threshold in seconds for each distance evaluation. If evaluation
     * shouldtake more time, it is interrupted and maximum disatcne is returned.
     */
    private final float timeThresholdForEval;

    private static native void init(String archiveDirectory, String preloadList, boolean binaryArchive, double inherentApprox);

    private native float getNativeDistance(String id1, String id2, float timeThresholdInSeconds, boolean storeLongToCache);
    
    /**
     *
     * @param timeThresholdForEval time threshold for the distanec computation.
     * If is exceeded, returns distance 3. Set -1 for unlimited.
     */
    public ProteinNativeQScoreDistance(float timeThresholdForEval) {
        if (timeThresholdForEval > 3600) {
            throw new IllegalArgumentException("Time threshold " + timeThresholdForEval + " is higher than allowed (3600 s)");
        }
        this.timeThresholdForEval = timeThresholdForEval;
    }

    /**
     * Must be called before first evaluation. Objects from specified file are
     * read into the main memorys for more efficient distance evaluations
     *
     * @param gesamtLibraryPath
     * @param cachedObjectsInPlainText
     * @param innerParameterOnSizeDiff 0 for no check, 0.6 according tu us, 0.7
     * according to gesamt
     */
    public static void initDistance(String gesamtLibraryPath, String cachedObjectsInPlainText, float innerParameterOnSizeDiff) {
        try {
            System.loadLibrary("ProteinDistance");
            // parameter 0.6 is inherent parametr in C library that was examined to speed-up distance evaluation
            // while well approximating the geometric similarity of protein structures
            if (gesamtLibraryPath == null) {
                init("/mnt/data/PDBe_clone_binary", cachedObjectsInPlainText, true, innerParameterOnSizeDiff);
            } else {
                init(gesamtLibraryPath, cachedObjectsInPlainText, true, innerParameterOnSizeDiff);
            }
        } catch (UnsatisfiedLinkError ex) {
            LOG.log(Level.WARNING, "Initialization of the distance function not successfull.");
            ex.printStackTrace();
        }

    }

    /**
     * Must be called before first evaluation. Objects from specified file are
     * read into the main memorys for more efficient distance evaluations
     *
     * @param gesamtLibraryPath
     * @param cachedObjectsInPlainText
     */
    public static void initDistance(String gesamtLibraryPath, String cachedObjectsInPlainText) {
        ProteinNativeQScoreDistance.initDistance(gesamtLibraryPath, cachedObjectsInPlainText, IMPLICIT_INNER_PARAMETER_ON_SIZE_CHECK);
    }

    @Override
    public float getDistance(String o1, String o2, float threshold) {
        if (o1 != null && o1.equals(o2)) {
            return 0;
        }
        try {
            return getNativeDistance(o1, o2, timeThresholdForEval, true);
        } catch (Exception e) {
            LOG.log(Level.SEVERE, "Unsuccessful attempt to evaluate distance between protein structures with IDs {0}, {1}", new Object[]{o1, o2});
            throw new IllegalArgumentException("Unsuccessful attempt to evaluate distance between protein structures with IDs " + o1 + ", " + o2);
        }
//        try {
//            Random r = new Random();
//            long l = Math.abs(r.nextLong() % 10000);
//            synchronized (r) {
//                r.wait(l);
//            }
//            i++;
//        } catch (InterruptedException ex) {
//            LOG.log(Level.SEVERE, null, ex);
//        }
//        return i;
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
