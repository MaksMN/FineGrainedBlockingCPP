#pragma once
#include <thread>
#include <mutex>
#include <iostream>

struct Node
{
    Node(int data) : value(data), next(nullptr) { node_mutex = new std::mutex; }
    ~Node() { delete node_mutex; }
    int value;
    Node *next;
    std::mutex *node_mutex;
};

class FineGrainedQueue
{
    Node *head;
    std::mutex *queue_mutex = new std::mutex;

public:
    ~FineGrainedQueue();
    void push_front(int data);
    void push_back(int data);
    void insert(int pos, int data);
    void remove(int value);
};