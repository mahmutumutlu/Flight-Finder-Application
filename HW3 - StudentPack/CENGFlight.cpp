#include "CENGFlight.h"
#include <iostream>

//=======================//
// Implemented Functions //
//=======================//
void CENGFlight::PrintCanNotHalt(const std::string& airportFrom,
                                 const std::string& airportTo,
                                 const std::string& airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines is not found and cannot be halted"
              << std::endl;
}

void CENGFlight::PrintCanNotResumeFlight(const std::string& airportFrom,
                                         const std::string& airportTo,
                                         const std::string& airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines cannot be resumed"
              << std::endl;
}

void CENGFlight::PrintFlightFoundInCache(const std::string& airportFrom,
                                         const std::string& airportTo,
                                         bool isCostWeighted)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              <<  " is found in cache." << std::endl;
}

void CENGFlight::PrintFlightCalculated(const std::string& airportFrom,
                                       const std::string& airportTo,
                                       bool isCostWeighted)
{
    std::cout << "A flight path is calculated between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              <<  "." << std::endl;
}

void CENGFlight::PrintPathDontExist(const std::string& airportFrom,
                                    const std::string& airportTo)
{
    std::cout << "A flight path does not exists between \""
              << airportFrom << "\" and \""
              << airportTo <<"\"." << std::endl;
}

void CENGFlight::PrintSisterAirlinesDontCover(const std::string& airportFrom)
{
    std::cout << "Could not able to generate sister airline list from \""
              << airportFrom <<"\"." << std::endl;
}

void CENGFlight::PrintMap()
{
    navigationMap.PrintEntireGraph();
}

void CENGFlight::PrintCache()
{
    lruTable.PrintTable();
}

CENGFlight::CENGFlight(const std::string& flightMapPath)
    : navigationMap(flightMapPath)
{}

//=======================//
//          TODO         //
//=======================//
void CENGFlight::HaltFlight(const std::string& airportFrom,
                            const std::string& airportTo,
                            const std::string& airlineName)
{
    
    HaltedFlight halt;
    halt.airportFrom = airportFrom;
    halt.airportTo = airportTo;
    halt.airline = airlineName;
    
    if (navigationMap.GetWeights(halt.w0, halt.w1, airportFrom, airportTo, airlineName)){
    navigationMap.RemoveEdge(airlineName, airportFrom, airportTo);
    
    haltedFlights.push_back(halt);
    }
    
    else PrintCanNotHalt(airportFrom, airportTo, airlineName);
    /* TODO */
}

    // (Direct Function call)
void CENGFlight::ContinueFlight(const std::string& airportFrom,
                                const std::string& airportTo,
                                const std::string& airlineName)
{
    int ind = 0;
    
    for (HaltedFlight halt: haltedFlights){
        
        if (halt.airportFrom == airportFrom &&
            halt.airportTo == airportTo &&
            halt.airline == airlineName){
            
            navigationMap.AddEdge(airlineName,
                                  airportFrom,
                                  airportTo,
                                  halt.w0,
                                  halt.w1);
            
            break;
        }
        
    }
    
    if (ind == haltedFlights.size())
    PrintCanNotResumeFlight(airportFrom, airportTo, airlineName);
    
    else haltedFlights.erase(haltedFlights.begin() + ind);
    /* TODO */
}

void CENGFlight::FindFlight(const std::string& startAirportName,
                            const std::string& endAirportName,
                            float alpha)
{
    std::vector<int> ove;
    
    if (alpha == 0.0f){
        
        if (navigationMap.HeuristicShortestPath(ove, startAirportName, endAirportName, alpha)){
            
            if (lruTable.Find(ove, ove.front(), ove.back(), true, true)){
                
                PrintFlightFoundInCache(startAirportName, endAirportName, true);
                navigationMap.PrintPath(ove, alpha, true);
                return;
                
            }
            
            PrintFlightCalculated(startAirportName, endAirportName, true);
            navigationMap.PrintPath(ove, alpha, true);
            
            lruTable.Insert(ove, true);
            
        }
        
        else PrintPathDontExist(startAirportName, endAirportName);
        
    }
    
    else if (alpha == 1.0f){
        
        if (navigationMap.HeuristicShortestPath(ove, startAirportName, endAirportName, alpha)){
            
            if (lruTable.Find(ove, ove.front(), ove.back(), false, true)){
                
                PrintFlightFoundInCache(startAirportName, endAirportName, false);
                navigationMap.PrintPath(ove, alpha, true);
                return;
                
            }
            
            PrintFlightCalculated(startAirportName, endAirportName, false);
            navigationMap.PrintPath(ove, alpha, true);
            
            lruTable.Insert(ove, true);
            
        }
        
        else PrintPathDontExist(startAirportName, endAirportName);
        
    }
    
    else{
        
        if (navigationMap.HeuristicShortestPath(ove, startAirportName, endAirportName, alpha)){
            
            navigationMap.PrintPath(ove, alpha, true);
            
        }
        
        else PrintPathDontExist(startAirportName, endAirportName);
        
    }
    
    /* TODO */
}

void CENGFlight::FindSpecificFlight(const std::string& startAirportName,
                                    const std::string& endAirportName,
                                    float alpha,
                                    const std::vector<std::string>& unwantedAirlineNames) const
{
    std::vector<int> ove;
    
    if (navigationMap.FilteredShortestPath(ove, startAirportName, endAirportName,
                                           alpha, unwantedAirlineNames)){
        
        navigationMap.PrintPath(ove, alpha, true);
        
    }
    /* TODO */
}

void CENGFlight::FindSisterAirlines(std::vector<std::string>& airlineNames,
                               const std::string& startAirlineName,
                               const std::string& airportName) const
{
    int size = navigationMap.GetSize();
    int ind = navigationMap.FindVertex(airportName);
    
    if (ind == -1) PrintSisterAirlinesDontCover(airportName);
    
    std::string airline = startAirlineName;
    
    std::vector<bool> marked (size, false);
    std::vector<std::string> airlines;
    std::vector<GraphEdge> edges;
    
    marked[ind] = true;
    navigationMap.GetEdges(edges, ind);
    
    for (int i = 0; i < size; i++){
        
        for (GraphEdge e: edges){
            
            if (e.name == airline) marked[e.endVertexIndex] = true;
            
        }
        
        airlines.push_back(airline);
        
        int cnt1 = 0, cnt2;
        for (int j = 0; j < size; j++){
            
            if (marked[j]){
                
                cnt2 = 0;
                navigationMap.GetEdges(edges, j);
                std::vector<bool> markedEdge (size, false);
                for (GraphEdge e: edges){
                    
                    if (!marked[e.endVertexIndex] && !markedEdge[e.endVertexIndex]){
                        markedEdge[e.endVertexIndex] = true;
                        cnt2++;
                    }
                }
                
                if (cnt1 < cnt2) ind = j;
                
            }
            
        }
        
        navigationMap.GetEdges(edges, ind);
        
        bool ctrlEdge = true;
        
        for (GraphEdge e: edges){
            
            bool ctrl = true;
            
            for (std::string name: airlines){
                
                if (e.name == name || marked[e.endVertexIndex]){
                    ctrl = false;
                    break;
                }
                
            }
            
            if(ctrl) {
                airline = e.name;
                ctrlEdge = false;
                break;
            }
            
        }
        
        if (ctrlEdge) break;
        
    }
    airlineNames = airlines;
    /* TODO */
}

int CENGFlight::FurthestTransferViaAirline(const std::string& airportName,
                                           const std::string& airlineName) const
{
    int depth = -1;
    
    if (navigationMap.FindVertex(airportName) != -1)
    depth = navigationMap.MaxDepthViaEdgeName(airportName, airlineName);
    /* TODO */
    return depth;
}