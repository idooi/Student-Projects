import java.util.Scanner;

public class Cluster {

    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        int[]  input = new int[0];
        int i = -1;
        int lastInput = scan.nextInt();
        if (lastInput == -1){
            return;
        }
        while(lastInput >= 0){
            i++;
            if (i >= input.length){
                input = ExtendArray(input, input.length + 1);
            }
            input[i] = lastInput;
            lastInput = scan.nextInt();
        }
        if (input.length == 1){
            System.out.print(input[0] + "\n");
            return;
        }
        int[] clusters = collectClusters(input);
        for(int j = 0; j < clusters.length; j++){
            System.out.print(clusters[j] + "\n");
        }
    }
    public static int[] ExtendArray(int[] array, int newLength){
        int[] newArray = new int[newLength];
        for (int i = 0; i < array.length; i++){
            newArray[i] = array[i];
        }
        return newArray;
    }
    public static int[] collectClusters(int[] input){
        MergeSort(input, input.length);
        int[] clusters = new int [0];
        int clusterStart = 0;
        int clusterCount = 0;
        for (int i = 1; i< input.length ; i++){
            if (input[i] - input[i - 1] > 1){ //new cluster
                clusterCount++;
                if (clusterCount > clusters.length){
                    clusters = ExtendArray(clusters, clusters.length + 1);
                }
                clusters[clusterCount - 1] = GetMedian(input, clusterStart,  i - 1);
                clusterStart = i;
            }
            if(i == input.length - 1) { //end
                clusterCount++;
                if (clusterCount > clusters.length){
                    clusters = ExtendArray(clusters, clusters.length + 1);
                }
                clusters[clusterCount - 1] = GetMedian(input, clusterStart, i);
                clusterStart = i;

            }
        }
        return clusters;
    }
    public static int GetMedian(int[] input, int start, int end){
        int length = (end - start) + 1;
        int medianIndex = (length % 2 == 0 ? start + length/2 - 1 : (start + end)/2);
        return input[medianIndex];
    }

    public static void MergeSort(int[] a, int length){
        if (length == 1){
            return;
        }
        int lLen = length/2;
        int rLen = length - lLen;
        // make temp arrays because java doesnt have pointer arithmetic because its stupid
        int[] la = new int[lLen];
        int[] ra = new int[rLen];
        for (int i = 0; i < lLen; i++){
            la[i] = a[i];
            ra[i] = a[i + lLen];
        }
        ra[rLen-1] = a[length - 1]; //if length is odd
        MergeSort(la, lLen);
        MergeSort(ra, rLen );
        Merge(a, length, la, ra);
    }
    public static void Merge(int[] a, int length, int[] b, int[] c){
        int bLen = length/2, cLen = length-length/2;
        int i = 0, ib = 0, ic = 0;
        while(i < length){
            if (ib < bLen && ic < cLen){//both alive
                if(b[ib] < c[ic]){
                    a[i] = b[ib];
                    ib++;
                }
                else {
                    a[i] = c[ic];
                    ic++;
                }
            }
            else if(ib >= bLen && ic < cLen){// only bLen is dead
                a[i] = c[ic];
                ic++;
            }
            else if(ib < bLen && ic >= cLen) {// only cLen is dead
                a[i] = b[ib];
                ib++;
            }
            i++;
        }
    }
}
