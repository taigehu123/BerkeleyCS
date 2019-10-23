

import java.util.LinkedList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.PriorityQueue;


/**
 * This class provides a shortestPath method for finding routes between two points
 * on the map. Start by using Dijkstra's, and if your code isn't fast enough for your
 * satisfaction (or the autograder), upgrade your implementation by switching it to A*.
 * Your code will probably not be fast enough to pass the autograder unless you use A*.
 * The difference between A* and Dijkstra's is only a couple of lines of code, and boils
 * down to the priority you use to order your vertices.
 */
public class Router {
    /**
     * Return a LinkedList of <code>Long</code>s representing the shortest path from st to dest,
     * where the longs are node IDs.
     */


    public static LinkedList<Long> shortestPath(GraphDB g, double stlon,
                                                double stlat, double destlon, double destlat) {
        Router sp = new Router();
        return sp.aStarProcess(g, stlon, stlat, destlon, destlat);
    }

    private double heuristicComparator(GraphDB g, long id1, long id2, HashMap bestDistances,
                                       HashMap bestParents, double destLon, double destLat) {
        double parentDistance1 = (double) bestDistances.get(bestParents.get(id1));
        double parentDistance2 = (double) bestDistances.get(bestParents.get(id2));
        double distanceToEnd1 = g.distance(id1, g.closest(destLon, destLat));
        double distanceToEnd2 = g.distance(id2, g.closest(destLon, destLat));
        double distance1 = distanceToEnd1
                + g.distance(id1, (long) bestParents.get(id1)) + parentDistance1;
        double distance2 = distanceToEnd2
                + g.distance(id2, (long) bestParents.get(id2)) + parentDistance2;
        if (distance1 > distance2) {
            return 1;
        } else if (distance1 < distance2) {
            return -1;
        } else {
            return 0;
        }
    }

    private LinkedList<Long> aStarProcess(GraphDB g, Double stlon,
                                          Double stlat, Double destlon, Double destlat) {
        HashMap<Long, Double> bestDistance = new HashMap<>();
        HashMap<Long, Long> bestParent = new HashMap<>();
        HashSet<Long> markedNodes = new HashSet<>();
        LinkedList<Long> toReturn = new LinkedList<>();
        HeuristicComparator pqHelper =
                new HeuristicComparator(g, bestDistance, bestParent, destlon, destlat);
        PriorityQueue<Long> fringe = new PriorityQueue<>(pqHelper);
        long id = g.closest(stlon, stlat);
        fringe.add(id);
        bestDistance.put(id, 0.0);
        long endID = g.closest(destlon, destlat);
        while (fringe.peek() != endID) {
            Long currentNode = fringe.poll();
            double fastestWay = bestDistance.get(currentNode);
            for (long toLook : g.adjacent(currentNode)) {
                double aStarCalculator = fastestWay
                        + g.distance(currentNode, toLook);
                if (markedNodes.contains(toLook)) {
                    if (aStarCalculator < bestDistance.get(toLook)) {
                        bestParent.put(toLook, currentNode);
                        bestDistance.put(toLook, aStarCalculator);
                        fringe.add(toLook);
                    }
                } else {
                    markedNodes.add(toLook);
                    bestDistance.put(toLook, aStarCalculator);
                    bestParent.put(toLook, currentNode);
                    fringe.add(toLook);
                }
            }
        }
        long finalDest = fringe.peek();
        toReturn.add(finalDest);
        while (finalDest != id) {
            toReturn.addFirst(bestParent.get(finalDest));
            finalDest = bestParent.get(finalDest);
        }

        return toReturn;
    }

}





