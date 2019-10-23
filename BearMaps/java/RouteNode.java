import com.sun.corba.se.impl.orbutil.graph.Graph;
import java.util.HashMap;

/**
 * Created by taigehu on 4/20/17.
 */
public class RouteNode implements Comparable<RouteNode>{
    public long id;
    public double lon;
    public double lat;
    public RouteNode prev;
    public double destLon;
    public double destLat;
    public HashMap<Long, Double> bestDistances;

    public RouteNode(long id, double lon, double lat, RouteNode prev,
                     double destLon, double destLat, HashMap bestDistances) {
        this.id = id;
        this.lon = lon;
        this.lat = lat;
        this.prev = prev;
        this.destLon = destLon;
        this.destLat = destLat;
        this.bestDistances = bestDistances;

    }

    public double distance(double lon2, double lat2) {
        return Math.sqrt((lon -lon2)*(lon - lon2) + (lat - lat2)*(lat - lat2));
    }

    @Override
    public int compareTo(RouteNode node) {
        double parentDistance1 = bestDistances.get(this.prev.id);
        double parentDistance2 = bestDistances.get(node.prev.id);
        double distance1 = distance(destLon, destLat) + distance(prev.lon, prev.lat) + parentDistance1;
        double distance2 = node.distance(destLon, destLat) +
                node.distance(node.prev.lon, node.prev.lat) + parentDistance2;
        if (distance1 > distance2) {
            return 1;
        } else if (distance1 < distance2) {
            return -1;
        } else {
            return 0;
        }
    }
}
