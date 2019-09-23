class ProteinDistance {
    private native float getDistance(String o1id, String o2id, float timeThresholdInSeconds);
    public static void main(String[] args) {
    	ProteinDistance a = new ProteinDistance();
    	System.out.println(a.getDistance("19HC:A", "12AS:A", 0.5f));
    	System.out.println(a.getDistance("12AS:A", "19HC:A", 0.5f));
    	System.out.println(a.getDistance("153L:A", "153L:A", 0.5f));
    	System.out.println(a.getDistance("5DBL:A", "5HUZ:B", 0.5f));
    	System.out.println(a.getDistance("5J8V:A", "5TAS:G", 0.5f));
    }
    static {
        System.loadLibrary("ProteinDistance");
    }
}
