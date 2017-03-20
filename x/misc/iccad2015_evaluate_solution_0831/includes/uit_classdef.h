/******************************************************************************
 *                                                                            *
 * Copyright (c) 2015, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef UIT_CLASSDEF_H_
#define UIT_CLASSDEF_H_

namespace uit {

class Cell;
class Cellpin;
class CellpinIterator;
class CellTimingArcIterator;
class CellLib;
class Circuit;
class CircuitInstanceIterator;
class CircuitNetIterator;
class CircuitPinIterator;
class CircuitPrimaryInputIterator;
class CircuitPrimaryOutputIterator;
class ClockTree;
class Command;
class Edge;
class Instance;
class LUTTemplate;
class LibraryCellIterator;
class MurMurHash32;
class Net;
class NetPinlistIterator;
class Node;
class CPPR;
class CPPRStorage;
class PrefixHeap;
class PathPrefix;
class PathTrace;
class PathTraceMinMaxHeap;
class Pin;
class Pipeline;
class PrimaryInput;
class PrimaryOutput;
class RCTree;
class RCTreeEdge;
class RCTreeNode;
class Jump;
class Spef;
class SpefPin;
class SpefNet;
class SpefRes;
class SpefCap;
class SuffixTree;
class String;
class Test;
class Testpoint;
class Timer;
class TimerNodeIterator;
class TimerEdgeIterator;
class Timing;
class TimingArc;
class TimingArcIterator;
class TimingLUT;
class Verilog;
class VerilogInstance;
class VerilogWire;
class VerilogInput;
class VerilogOutput;
class VerilogInstanceCellpinToNetMappingIterator;
class VerilogModule;
class VerilogModuleIterator;
class VerilogModuleInputIterator;
class VerilogModuleInstanceIterator;
class VerilogModuleOutputIterator;
class VerilogModuleWireIterator;
class xxHash32;
class xxHash64;

template <class T> class Data; 
template <class V> class DynamicSet;
template <class T> class DynamicList;
template <class T> class List;
template <class T> class IndexedSet;
template <class T> class ListNode;
template <class M, class B> class Unit; 
template <class K, class V> class Dictionary;

};


#endif
