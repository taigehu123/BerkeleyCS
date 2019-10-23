
import org.xml.sax.SAXException;
import java.util.HashMap;
import java.io.File;
import java.io.IOException;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import java.util.ArrayList;



/**
 * Graph for storing all of the intersection (vertex) and road (edge) information.
 * Uses your GraphBuildingHandler to convert the XML files into a graph. Your
 * code must include the vertices, adjacent, distance, closest, lat, and lon
 * methods. You'll also need to include instance variables and methods for
 * modifying the graph (e.g. addNode and addEdge).
 *
 * @author Alan Yao, Josh Hug
 */
public class GraphDB {
    /** Your instance variables for storing the graph. You should consider
     * creating helper classes, e.g. Node, Edge, etc. */

    /**
     * Example constructor shows how to create and start an XML parser.
     * You do not need to modify this constructor, but you're welcome to do so.
     * @param dbPath Path to the XML file to be parsed.
     */

    private HashMap<Long, GraphNode> nodeTracker;
    private HashMap<Long, ArrayList<Long>> nodeConnections;
    private ArrayList<Long> emptyNodes;

    public GraphDB(String dbPath) {
        nodeTracker = new HashMap<>();
        nodeConnections = new HashMap<>();
        emptyNodes = new ArrayList<>();
        try {
            File inputFile = new File(dbPath);
            SAXParserFactory factory = SAXParserFactory.newInstance();
            SAXParser saxParser = factory.newSAXParser();
            GraphBuildingHandler gbh = new GraphBuildingHandler(this);
            saxParser.parse(inputFile, gbh);
        } catch (ParserConfigurationException | SAXException | IOException e) {
            e.printStackTrace();
        }
        clean();
    }

    /**
     * Helper to process strings into their "cleaned" form, ignoring punctuation and capitalization.
     * @param s Input string.
     * @return Cleaned string.
     */
    static String cleanString(String s) {
        return s.replaceAll("[^a-zA-Z ]", "").toLowerCase();
    }

    /**
     *  Remove nodes with no connections from the graph.
     *  While this does not guarantee that any two nodes in the remaining graph are connected,
     *  we can reasonably assume this since typically roads are connected.
     */
    private void clean() {
        for (long key: emptyNodes) {
            nodeTracker.remove(key);
            nodeConnections.remove(key);
        }
    }

    /** Returns an iterable of all vertex IDs in the graph. */
    Iterable<Long> vertices() {
        //YOUR CODE HERE, this currently returns only an empty list.
        return nodeTracker.keySet();
    }

    /** Returns ids of all vertices adjacent to v. */
    Iterable<Long> adjacent(long v) {
        return nodeConnections.get(v);
    }

    /** Returns the Euclidean distance between vertices v and w, where Euclidean distance
     *  is defined as sqrt( (lonV - lonV)^2 + (latV - latV)^2 ). */
    double distance(long v, long w) {

        return Math.sqrt((lon(v) - lon(w)) * (lon(v) - lon(w))
                + (lat(v) - lat(w)) * (lat(v) - lat(w)));
    }

    /** Returns the vertex id closest to the given longitude and latitude. */
    long closest(double lon, double lat) {
        Double closestDistance = null;
        long toReturn = 0;
        for (long id: nodeTracker.keySet()) {
            double lon1 = nodeTracker.get(id).lon;
            double lat1 = nodeTracker.get(id).lat;
            double checker = Math.sqrt((lon1 - lon) * (lon1 - lon) + (lat1 - lat) * (lat1 - lat));
            if (closestDistance == null || checker < closestDistance) {
                closestDistance = checker;
                toReturn = id;
            }
        }
        return toReturn;
    }

    /** Longitude of vertex v. */
    double lon(long v) {
        return nodeTracker.get(v).lon;
    }

    double lat(long v) {
        return nodeTracker.get(v).lat;
    }

    void addNode(long id, double lat, double lon) {
        if (!nodeTracker.containsKey(id)) {
            GraphNode toAdd = new GraphNode(id, lon, lat);
            emptyNodes.add(id);
            nodeTracker.put(id, toAdd);
        }
        if (!nodeConnections.containsKey(id)) {
            nodeConnections.put(id, new ArrayList<>());
        }
    }

    void addConnection(long a, long b) {
        nodeConnections.get(a).add(b);
        nodeConnections.get(b).add(a);
        if (emptyNodes.contains(a)) {
            emptyNodes.remove(a);
        }
        if (emptyNodes.contains(b)) {
            emptyNodes.remove(b);
        }
    }

    int size() {
        return nodeTracker.size();
    }



    private class GraphNode {
        long id;
        double lat;
        double lon;

        GraphNode(long id, double lon, double lat) {
            this.id = id;
            this.lat = lat;
            this.lon = lon;
        }
    }
}
