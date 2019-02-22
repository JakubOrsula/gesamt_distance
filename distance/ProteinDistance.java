class ProteinDistance {
    private native float getDistance(String o1id, String o2id, float threshold);
    public static void main(String[] args) {
    	ProteinDistance a = new ProteinDistance();
    	System.out.println(a.getDistance("19HC:A", "12AS:A", 0));
    	System.out.println(a.getDistance("12AS:A", "19HC:A", 0));
    	System.out.println(a.getDistance("153L:A", "153L:A", 0));
    }
    static {
        System.loadLibrary("ProteinDistance");
    }
}
