#include "FineGrainedQueue.h"

FineGrainedQueue::~FineGrainedQueue()
{
    delete queue_mutex;
}

void FineGrainedQueue::push_front(int data)
{
    // создаем новый узел
    Node *node = new Node(data);
    // если список пуст, возвращаем узел
    if (head == nullptr)
    {
        head = node;
        return;
    }
    // в цикле ищем последний элемент списка
    Node *last = head;
    while (last->next != nullptr)
    {
        last = last->next;
    }
    // Обновляем указатель next последнего узла на указатель на новый узел
    last->next = node;
}

void FineGrainedQueue::push_back(int data)
{
    // создаем новый узел
    Node *node = new Node(data);
    // если список пуст, возвращаем узел
    if (head == nullptr)
    {
        head = node;
        return;
    }
    // в цикле ищем последний элемент списка
    Node *last = head;
    while (last->next != nullptr)
    {
        last = last->next;
    }
    // Обновляем указатель next последнего узла на указатель на новый узел
    last->next = node;
    return;
}

void FineGrainedQueue::insert(int pos, int data)
{
    // здесь надо сделать так чтобы соседние потоки не уничтожили связь current->next и сам current
    queue_mutex->lock(); // залочим весь список

    Node *newNode = new Node(data); // новый вставляемый узел
    Node *current = head;           // на старте текущий узел = головной узел

    // если весь список залочен нашим потоком то текущий и следующий элементы
    // не могут быть удалены другими потоками

    // залочим текущий и следующий узлы
    current->node_mutex->lock();

    current->next->node_mutex->lock();

    // освободим очередь.
    queue_mutex->unlock();

    int currPos = 0;

    // в цикле идем по списку, пока список не кончится, или пока не дойдем до позиции
    while (currPos < pos - 1 && current->next != nullptr)
    {
        Node *old_current = current; // залочен, надо освободить
        current = current->next;     // залочен, не надо освобождать

        // current->next не залочена, может быть удалена другими потоками, надо ее залочить
        if (current->next)
        {
            current->next->node_mutex->lock();
        }

        old_current->node_mutex->unlock();
        currPos++;
    }
    // меняем указатель на следующий узел на указатель на новый узел
    Node *next = current->next;
    current->next = newNode;
    // связываем список обратно, меняем указатель на узел, следующий после нового узла, на указатель на узел, следующий за current
    newNode->next = next;

    // разлочиваем
    newNode->node_mutex->unlock();
    current->node_mutex->unlock();
    if (next)
    {
        next->node_mutex->unlock();
    }
}

void FineGrainedQueue::remove(int value)
{
    Node *prev, *cur;
    queue_mutex->lock();

    // здесь должна быть обработка случая пустого списка

    prev = this->head;
    cur = this->head->next;

    prev->node_mutex->lock();
    // здесь должна быть обработка случая удаления первого элемента
    queue_mutex->unlock();

    if (cur) // проверили и только потом залочили
        cur->node_mutex->lock();
    while (cur)
    {
        if (cur->value == value)
        {
            prev->next = cur->next;
            prev->node_mutex->unlock();
            cur->node_mutex->unlock();
            delete cur;
            return;
        }
        Node *old_prev = prev;
        prev = cur;
        cur = cur->next;
        old_prev->node_mutex->unlock();
        if (cur) // проверили и только потом залочили
            cur->node_mutex->lock();
    }
    prev->node_mutex->unlock();
}
