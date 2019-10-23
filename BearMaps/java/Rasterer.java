import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Collections;
import java.util.Arrays;

/**
 * This class provides all code necessary to take a query box and produce
 * a query result. The getMapRaster method must return a Map containing all
 * seven of the required fields, otherwise the front end code will probably
 * not draw the output correctly.
 */
public class Rasterer {
    // Recommended: QuadTree instance variable. You'll need to make
    //              your own QuadTree since there is no built-in quadtree in Java.
    private QuadTree root;

    /** imgRoot is the name of the directory containing the images.
     *  You may not actually need this for your class. */
    public Rasterer(String imgRoot) {
        this.root = new QuadTree(MapServer.ROOT_ULLON, MapServer.ROOT_ULLAT,
                MapServer.ROOT_LRLON, MapServer.ROOT_LRLAT, 0, 0);
    }

    public Map<String, Object> getMapRaster(Map<String, Double> params) {
        Map<String, Object> results = new HashMap<>();
        ArrayList<QuadTree> correctNodes = new ArrayList<>();
        double lonDPP = (params.get("lrlon") - params.get("ullon")) / params.get("w");
        renderGrid(params, lonDPP, root, correctNodes);
        Collections.sort(correctNodes);
        String[][] result = new String[lengthRow(correctNodes)][correctNodes.size()
                / lengthRow(correctNodes)];
        sortGrid(correctNodes, result);
        results.put("render_grid",  result);
        results.put("raster_ul_lon", correctNodes.get(0).ullon);
        results.put("raster_ul_lat", correctNodes.get(0).ullat);
        results.put("raster_lr_lon", correctNodes.get(correctNodes.size() - 1).lrlon);
        results.put("raster_lr_lat", correctNodes.get(correctNodes.size() - 1).lrlat);
        results.put("depth", correctNodes.get(0).depth());
        results.put("query_success", true);
        return results;
    }

    private void renderGrid(Map<String, Double> params, double queryDPP,
                            QuadTree toAdd, ArrayList<QuadTree> correctNodes) {

        if (!toAdd.ifIntersect(params.get("ullon"), params.get("ullat"),
                params.get("lrlon"), params.get("lrlat"))) {
            return;
        } else {
            if (!toAdd.lonDPPsmallerThanOrIsLeaf(queryDPP)) {
                renderGrid(params, queryDPP, toAdd.upperLeft, correctNodes);
                renderGrid(params, queryDPP, toAdd.upperRight, correctNodes);
                renderGrid(params, queryDPP, toAdd.lowerLeft, correctNodes);
                renderGrid(params, queryDPP, toAdd.lowerRight, correctNodes);
            } else {
                correctNodes.add(toAdd);
                return;
            }
        }
    }

    private void sortGrid(ArrayList<QuadTree> correctNodes, String[][] toSort) {
        int i = lengthRow(correctNodes);
        for (int n = 0; n < i; n++) {
            String[] toAdd = new String[correctNodes.size() / i];
            for (int k = 0; k < correctNodes.size() / i; k++) {
                toAdd[k] = correctNodes.get(toAdd.length * n + k).stringForm();
            }
            Arrays.sort(toAdd);
            toSort[n] = toAdd;
        }

    }

    private int lengthRow(ArrayList<QuadTree> correctNodes) {
        double initialLat = correctNodes.get(0).ullat;
        int i = 1;
        for (QuadTree quadTree: correctNodes) {
            if (!(quadTree.ullat == initialLat)) {
                i++;
                initialLat = quadTree.ullat;
            }
        }
        return i;
    }

}
