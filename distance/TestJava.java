import messif.distance.impl.ProteinNativeQScoreDistance;

public class TestJava {

    public static void main (String [] args) {

        ProteinNativeQScoreDistance distance = new ProteinNativeQScoreDistance(0.6f);

        if (args.length < 4) {
            System.err.println("Insufficient number of arguments.");
            System.exit(1);
        }

        String archive_dir = args[0];
        String preload_list = args[1];

        String id1 = args[2];
        String id2 = args[3];

        distance.initDistance(archive_dir, preload_list);
        float d = distance.getDistance(id1, id2, 1.0f);
        System.out.println(d);
    }
}