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

        String query = args[2];

        distance.initDistance(archive_dir, preload_list);

        for (int i = 3; i < args.length; i++) {
            System.out.println(distance.getDistance(query, args[i], 0.3f));
        }
    }
}