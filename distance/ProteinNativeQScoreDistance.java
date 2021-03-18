/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package messif.distance.impl;

import java.io.Serializable;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Vlada
 */
public class ProteinNativeQScoreDistance implements Serializable {

    /**
     * Class id for Java serialization.
     */
    private static final long serialVersionUID = 124687651462L;

    public static final Float IMPLICIT_INNER_PARAMETER_ON_SIZE_CHECK = 0.6f;
    private static final Logger LOG = Logger.getLogger(ProteinNativeQScoreDistance.class.getName());

    private static native void init(String archiveDirectory, double inherentApprox);

    private static native float[] getStats(String id1, String id2, float scoreThreshold);

    /**
     * Must be called before first evaluation. Objects from specified file are
     * read into the main memorys for more efficient distance evaluations
     *
     * @param gesamtLibraryPath
     * @param innerParameterOnSizeDiff 0 for no check, 0.6 according tu us, 0.7
     * according to gesamt
     */
    public static void initDistance(String gesamtLibraryPath, float innerParameterOnSizeDiff) {
        try {
            System.loadLibrary("ProteinDistance");
            // parameter 0.6 is inherent parametr in C library that was examined to speed-up distance evaluation
            // while well approximating the geometric similarity of protein structures
            if (gesamtLibraryPath == null) {
                init("/mnt/data/PDBe_clone_binary", innerParameterOnSizeDiff);
            } else {
                init(gesamtLibraryPath, innerParameterOnSizeDiff);
            }
        } catch (Exception ex) {
            LOG.log(Level.WARNING, "Initialization of the distance function not successfull.", ex);
        }

    }

    /**
     * Must be called before first evaluation. Objects from specified file are
     * read into the main memorys for more efficient distance evaluations
     *
     * @param gesamtLibraryPath
     */
    public static void initDistance(String gesamtLibraryPath) {
        initDistance(gesamtLibraryPath, IMPLICIT_INNER_PARAMETER_ON_SIZE_CHECK);
    }

    public float[] getStatsFloats(String o1, String o2, float distThreshold) {
        if (o1 == null || o2 == null) {
            LOG.log(Level.SEVERE, "Attempt to evaluate distance between null proteins {0}, {1}.", new Object[]{o1, o2});
        }
        try {
            distThreshold = Math.min(distThreshold, 1);
            float[] ret = getStats(o1, o2, 1 - distThreshold);
            if (o1.equals(o2)) {
                ret[0] = 1;
            }
            return ret;
        } catch (Exception e) {
            LOG.log(Level.SEVERE, "Unsuccessful attempt to evaluate distance between protein structures with IDs {0}, {1}. Original exception: {2}", new Object[]{o1, o2, e.getMessage()});
            throw new IllegalArgumentException("Unsuccessful attempt to evaluate distance between protein structures with IDs " + o1 + ", " + o2);
        }
    }

    @Override
    public String toString() {
        return getClass().getSimpleName();
    }

    public float getMaxDistance() {
        return 1f;
    }

}
