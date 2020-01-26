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
     * @param gesamtLibraryPath
     * @param cachedObjectsInPlainText
     */
    public static void initDistance(String gesamtLibraryPath, String cachedObjectsInPlainText) {
        try {
            System.loadLibrary("ProteinDistance");
            // parameter 0.6 is inherent parametr in C library that was examined to speed-up distance evaluation
            // while well approximating the geometric similarity of protein structures
            if (gesamtLibraryPath == null) {
                init("/mnt/data/PDBe_clone_binary", cachedObjectsInPlainText, true, 0.6);
            } else {
                init(gesamtLibraryPath, cachedObjectsInPlainText, true, 0.6);
            }
        } catch (UnsatisfiedLinkError ex) {
            Logger.getLogger(ProteinNativeQScoreDistance.class.getName()).log(Level.WARNING, "Inicialisation of the distance function not successfull.");
        }
    }

    int i = 0;

    @Override
    public float getDistance(String o1, String o2, float threshold) {
      return getNativeDistance(o1, o2, timeThresholdForEval);
//        try {
//            Random r = new Random();
//            long l = Math.abs(r.nextLong() % 10000);
//            synchronized (r) {
//                r.wait(l);
//            }
//            i++;
//        } catch (InterruptedException ex) {
//            Logger.getLogger(ProteinNativeQScoreDistance.class.getName()).log(Level.SEVERE, null, ex);
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
