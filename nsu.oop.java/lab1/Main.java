package oop.nsu.maxermolaev;

import java.io.*;
import java.util.*;

public class Main {
    public static void main(String[] args) throws IOException {
        String csvFile = "output.csv";
        StringBuilder line = new StringBuilder();
        Map<String, Integer> words = new TreeMap<>();
        //Map<String, Integer> words = new HashMap<>();
        int totalWords = 0;
        String path = "/Users/maxermolaev/IdeaProjects/lab1/input.txt";
        File file = new File(path);

        try (InputStreamReader isr = new InputStreamReader(new FileInputStream(file))){
            while (isr.ready()) {
                int symbol = isr.read();
                if (Character.isLetterOrDigit(symbol)) {
                    line.append((char) symbol);
                } else if (line.length() != 0) {
                    line = new StringBuilder(line.toString().toLowerCase());
                    words.put(line.toString(), words.getOrDefault(line.toString(), 0) + 1);
                    line = new StringBuilder();
                    totalWords++;
                }
            }
        }

        List<Lines> list = new ArrayList<>();
        for (String key : words.keySet()) {
            list.add(new Lines(key, words.get(key)));
        }
        Collections.sort(list);

        File outputFile = new File(csvFile);
        FileWriter pw = new FileWriter(outputFile);
        pw.write("Слово, Частота, Частота в %\n");
        for (Lines l : list) {
            pw.write((l.getWord() + ", " + l.getCount() + ", " + (l.getCount() * 100 / totalWords) + "%\n"));
        }
        pw.close();
    }
}
//        File outputFile = new File(csvFile);
//        PrintWriter pw = new PrintWriter(outputFile);
//        pw.println((l.getWord() + ", " + l.getCount() + ", " + (l.getCount() * 100 / totalWords) + "%")

//        ArrayList<Integer> keys = new ArrayList<>();
//
//        for(String key : words.keySet()){
//            keys.add(words.get(key));
//            }
//
//        System.out.println(keys.toString());
//        System.out.println(words);
//        System.out.println(totalWords);

//        Collections.sort(keys, Collections.reverseOrder());
//        System.out.println(keys.toString());
//        System.out.println(totalWords);
//        System.out.println(words);
//        System.out.println(words.containsValue(4));
//        for(Integer key : keys){
//
//        }


//        Scanner scanner = new Scanner(file);
//        for(int i = 1;i < 2; i++) {
//            String line = scanner.nextLine();
//            line = line.toLowerCase();
//            System.out.println(line);
// }

