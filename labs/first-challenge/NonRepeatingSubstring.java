import java.util.*;

public class NonRepeatingSubstring {

    public NonRepeatingSubstring(String input) {
        System.out.println(nonRepeatingSubstringLength(input));
    }

    private int nonRepeatingSubstringLength(String input) {
        Set<Character> charsInSubstring = new HashSet<>();
        StringBuilder current = new StringBuilder();

        int currentMaxLength = 0, j=0;

        for(int i=0; i<input.length(); i++){
            if(charsInSubstring.contains(input.charAt(i))){
                if(charsInSubstring.size() > currentMaxLength) {
                    currentMaxLength = charsInSubstring.size();
                }
                charsInSubstring = new HashSet<>();
                //System.out.println(current.toString());
                current= new StringBuilder();
                i = j++;
                
            }
            else{
                charsInSubstring.add(input.charAt(i));
                current.append(input.charAt(i));
            }
        }

        if(charsInSubstring.size() > currentMaxLength) {
            currentMaxLength = charsInSubstring.size();
        }
        //System.out.println(current.toString());

        return currentMaxLength;
    }

    public static void main(String[] args){
        new NonRepeatingSubstring(args[0]);
    }

}