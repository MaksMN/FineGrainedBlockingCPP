#include "FineGrainedQueue.h"

int main()
{
    FineGrainedQueue q;

    q.push_front(0);
    q.push_back(1);
    q.push_back(2);
    q.push_back(3);
    q.push_back(4);
    q.push_back(5);
    q.push_back(6);
    q.push_back(7);
    q.push_back(8);
    q.push_back(9);

    /* вставка двух элементов в одну и ту же позицию */
    /* при отсутствии гонки начало списка будет иметь вид: 0 - 222 - 111 */
    std::thread t1([&q]()
                   { q.insert(1, 111); });
    std::thread t2([&q]()
                   { q.insert(1, 222); });

    /* одновременное удаление и вставка в одну и ту же позицию. */
    /* отсутствие гонки данных: узлы со значениями 3 и 4 удалены, на их месте новые узлы со значениями 333 и 444 */
    /* эта область списка будет иметь вид 2 - 444 - 333 - 5 */
    std::thread t33([&q]()
                    { q.remove(3); });
    // первые два потока сместили тройку и четверку в 5 позицию.
    std::thread t3([&q]()
                   { q.insert(5, 333); });
    std::thread t333([&q]()
                     { q.remove(4); });
    std::thread t4([&q]()
                   { q.insert(5, 444); });

    /* эти потоки должны занять два новых последних места в списке. Позиция превышает длину списка. */
    /* эта область списка будет иметь вид 9 - 666 - 555 */
    std::thread t5([&q]()
                   { q.insert(222, 555); });
    std::thread t6([&q]()
                   { q.insert(222, 666); });
    t1.join();
    t2.join();
    t3.join();
    t33.join();
    t333.join();
    t4.join();
    t5.join();
    t6.join();

    return 0;
}