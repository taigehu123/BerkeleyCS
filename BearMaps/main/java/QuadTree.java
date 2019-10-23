/**
 * Created by taigehu on 4/17/17.
 */
public class QuadTree implements Comparable<QuadTree> {
    QuadTree upperLeft;
    QuadTree upperRight;
    QuadTree lowerLeft;
    QuadTree lowerRight;
    double ullon;
    double ullat;
    double lrlon;
    double lrlat;
    private int depth;
    private int number;


    public QuadTree(double ullon, double ullat, double lrlon, double lrlat, int depth, int label) {
        this.ullon = ullon;
        this.ullat = ullat;
        this.lrlon = lrlon;
        this.lrlat = lrlat;
        this.number = label;
        this.depth = depth;
        if (depth <= 7) {
            this.upperLeft = new QuadTree(ullon, ullat, halfway(ullon, lrlon),
                    halfway(ullat, lrlat), depth + 1, label * 10 + 1);
            this.upperRight = new QuadTree(halfway(ullon, lrlon), ullat,
                    lrlon, halfway(ullat, lrlat), depth + 1, label * 10 + 2);
            this.lowerLeft = new QuadTree(ullon, halfway(ullat, lrlat),
                    halfway(ullon, lrlon), lrlat, depth + 1, label * 10 + 3);
            this.lowerRight = new QuadTree(halfway(ullon, lrlon), halfway(ullat, lrlat),
                    lrlon, lrlat, depth + 1, label * 10 + 4);
        }
    }

    private double halfway(double first, double second) {
        return (first + second) / 2;
    }

    public String stringForm() {
        return "img/" + Integer.toString(this.number) + ".png";
    }

    public double lonDPP() {
        return (this.lrlon - this.ullon) / 256;
    }

    public int depth() {
        return this.depth;
    }

    public boolean lonDPPsmallerThanOrIsLeaf(double queryDPP) {
        return this.lonDPP() <= queryDPP || this.depth() == 7;
    }

    public boolean ifIntersect(double ullon, double ullat, double lrlon, double lrlat) {
        return !(this.ullon > lrlon || this.lrlon < ullon
                || this.ullat < lrlat || this.lrlat > ullat);
    }

    public int compareTo(QuadTree q) {
        if (this.ullat == q.ullat) {
            if (this.ullon > q.ullon) {
                return 1;
            } else if (this.ullon < q.ullon) {
                return -1;
            } else {
                return 0;
            }
        } else {
            if (this.ullat < q.ullat) {
                return 1;
            } else if (this.ullat > q.ullat) {
                return -1;
            } else {
                return 0;
            }
        }
    }


}
