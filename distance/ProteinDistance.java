class ProteinDistance {
    private native float getDistance(String o1id, String o2id, float timeThresholdInSeconds);
    private native void init(String archiveDirectory, String preloadList, boolean binaryArchive, double threshold);

    public static void main(String[] args) {
    	ProteinDistance a = new ProteinDistance();
    	a.init("/home/krab1k/Tmp/dst", "/home/krab1k/Tmp/sel", true, 0.0);
    	System.out.println(a.getDistance("2AZE:A", "1A03:B", 1.0f));
    }
    static {
        System.loadLibrary("ProteinDistance");
    }
}
