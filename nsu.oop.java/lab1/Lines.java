package oop.nsu.maxermolaev;

public class Lines implements Comparable<Lines> {
    private String word;
    private int count;

    public Lines(String word, int count) {
        this.word = word;
        this.count = count;
    }

    int getCount() {
        return count;
    }

    String getWord() {
        return word;
    }

    public int compareTo(Lines line) {
        if (count == line.getCount()) {
            return 0;
        } else if (count > line.getCount()) {
            return -1;
        } else {
            return 1;
        }
    }
}