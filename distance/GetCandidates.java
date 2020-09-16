import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;


public class GetCandidates {

    public static void main (String [] args) {

        if (args.length < 1) {
            System.err.println("Insufficient number of arguments.");
            System.exit(1);
        }

        String archive_dir = args[0];

        List<String> files = new ArrayList<String>();

        try {
            Files.walk(Paths.get(archive_dir))
                      .filter(Files::isRegularFile)
                      .forEach(file -> files.add(file.toString()));
        } catch (Exception e) {
            System.err.println("Error occurred when reading the archive directory");
            System.exit(2);
        }

        Collections.shuffle(files);

        int noOfCandidates = (int)(Math.random() * 1500 + 1);
        for(int i = 0; i < noOfCandidates; i++) {
            File f = new File(files.get(i));
            String name = f.getName();
            /* Strip '.bin' suffix */
            System.out.println(name.substring(0, name.length() - 4));
        }
    }
}