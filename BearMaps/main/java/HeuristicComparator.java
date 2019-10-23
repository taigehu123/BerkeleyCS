import com.sun.corba.se.impl.orbutil.graph.Graph;

import java.util.Comparator;
import java.util.HashMap;

/**
 * Created by taigehu on 4/20/17.
 */
public class HeuristicComparator implements Comparator<Long> {
    private GraphDB g;

    private HashMap bestDistances;
    private HashMap<Long, Long> bestParents;
    double destLon;
    double destLat;

    public HeuristicComparator(GraphDB g,HashMap bestDistances, HashMap bestParents, double destLon, double destLat) {
        this.g = g;
        this.bestDistances = bestDistances;
        this.bestParents = bestParents;
        this.destLon = destLon;
        this.destLat = destLat;

    }

    public int compare(Long id1, Long id2) {

        double thisLon = g.lon(id1);
        double thisLat = g.lat(id1);
        double thisLon2 = g.lon(id2);
        double thisLat2 = g.lat(id2);
        long parent1 = bestParents.get(id1);
        long parent2 = bestParents.get(id2);
        double parentDistance1 = (double)bestDistances.get(parent1);
        double parentDistance2 = (double)bestDistances.get(parent2);
        double distanceToEnd1 = distance(thisLon, destLon, thisLat, destLat);
        double distanceToEnd2 = distance(thisLon2, destLon, thisLat2, destLat);
        double distanceFromParent1 = distance(thisLon, g.lon(parent1), thisLat, g.lat(parent1));
        double distanceFromParent2 = distance(thisLon2, g.lon(parent2), thisLat2, g.lat(parent2));
        double distance1 = distanceToEnd1 + distanceFromParent1 + parentDistance1;
        double distance2 = distanceToEnd2 + distanceFromParent2 + parentDistance2;
        if (distance1 > distance2) {
            return 1;
        } else if (distance1 < distance2) {
            return -1;
        } else {
            return 0;
        }
    }

    private double distance(double lon1, double lon2, double lat1, double lat2) {
        return Math.sqrt((lon1 - lon2)*(lon1 - lon2) + (lat1 - lat2)*(lat1 - lat2));
    }
}
