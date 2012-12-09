
#include <iostream>
#include <vector>
using namespace std;

const vector<bool> TODOS(10, true);

class Posibles {
   vector<bool> _bits;
public:
   Posibles() : _bits(TODOS) {}
   bool activo(int i) const { return _bits[i]; }
   void elimina(int i) { _bits[i] = false; }
   void escribe(ostream& o) const {
      for (int i = 1; i < 10; i++) {
         if (_bits[i]) o << i;
      }
   }
};

inline ostream& operator<<(ostream& o, const Posibles& p) {
   p.escribe(o); return o;
}



int main() {
   Posibles p;
   cout << p << endl;
   p.elimina(1);
   p.elimina(3);
   cout << p << endl;
}
