#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

/*
2 dew:

1) wenn fertig:
alle "doubles" zu "key_type umaendern"
den kommentar bei template wegmachen



2) CHECK 2ter konstruktor ist nicht feritg
3) CHECK 3ter konstruktor ist nicht fertig
4) CHECK add fehlt
5) CHECK reserve fehlt
6) CHECK (HOFFENTLICH JETZT WIRKLICH) 4ter konstruktor ist 100% falsch
7) CHECK ADS_set &operator=(const ADS_set &other) ist auch 100% falsch aus dem gleichen grund wie 4ter konstruktor
8) for if do mit for do if austauschen? weil eventuell schneller?
9) CHECK reserve funktioniert nicht richtig. es wird nicht gerahasht am ende
10) CHECK add muss noch checken ob der neue eintrag nicht schon vorkommt
11) CHECK "size_t count(const double &data) const" stimmt nicht. es wird das datum nie gehasht. ausserdem kann ich einfach mit count suchen anstatt es in der add funktion zu machen
12) CHECK es wird nochmal %max_sz in add() gemacht obwohl schon %max_sz in hash() gemacht wird
13) CHECK dekonstruktor ~ADS_set() funktioniert nicht. delete muss die buckets der linked list auch loeschen
14) CHECK PH2 ADS_set &operator=(std::initializer_list<double> ilist) noch nicht implementiert
15) CHECK void clear() fehlt
16) CHECK PH2 size_t erase(const double &data_to_erase){} fehlt
17) CHECK PH2 void swap(ADS_set &other){} fehlt
18) CHECK PH2 std::pair<iterator,bool> insert(const key_type &key)
19) CHECK ka ob void clear() stimmt, auf cewebs ist die beschreibung relativ beschissen
20) weiss nicht ob ich bei ADS_set &operator=(std::initializer_list<key_type> ilist) die alten elemente rausloeschen muss oder nicht (also ob ich clear dazwischen aufrufen soll)
21) CHECK size_type erase(const key_type &data_to_erase) funktioniert nicht richtig
22) CHECK fix add: wenn ein erastes datum nochmal neu reinkommt wird es nicht geadded
23) CHECK letztes element erasen funktioniert nicht
24) CHECK Serstes element erasen wenns danach noch elemente gibt funktioniert nicht. spackt rum
25) CHECK find funktioniert nicht mal ansatzweise richtig. es findet GARNICHTS
26) CHECK std::pair<iterator,bool> insert(const key_type &key) funktioniert nicht
27) CHECK dekonstruktor hat somehow immer noch memory leaks :pepehands: nevermind der memory leak war in add. habe da den bucket_to_add nicht deleted wenns nicht gepasst hat
28) CHECK iterator ctor kaputt
29) CHECK std::pair<iterator,bool> insert(const key_type &data_to_be_inserted insertet anscheinend nicht
30) vielleicht std::pair<iterator,bool> insert(const key_type &data_to_be_inserted) umschreiben weil es ist nicht besonders effizient mit den breaks;. 
    vielleicht statt while(list_ptr->mode == Mode::used)    while(list_ptr->next_ptr != nullptr) machen?
31) CHECK friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) passt nicht. 
32) CHECK ADS_set &operator=(const ADS_set &other). clear() hat am anfang gefehlt
33) CHECK erase geht schon wieder nicht.
34) CHECK swap geht nicht. der fehler war in ADS_set &operator=(const ADS_set &other) und nicht in other (i think)
35) ADS_set &operator=(const ADS_set &other) ist nicht effizient weil ich clear gecallt habe und dann nochmal die table deleted habe und dann nochmal 
    die table mit der richtigen groesse erstellt habe. falls es performance probleme gibt eventuell aendern.
36) std::pair<iterator,bool> insert(const key_type &data_to_be_inserted) funzt nicht
37) CHECK MOTHERFUCKER B) swap immer noch kaputt
*/

/*
FERTIG. JETZT NUR NOCH SACHEN AUSBESSERN
1) CHECK AS FUCK LESSGO. SWAP IST SPEED AF swap ist giga langsam
2) operator == hasht unnoetig. kann man fixen
*/

/*
muss noch im nachhinein fixen:
operator
*/




template <typename Key, size_t N = 7>
class ADS_set
{

/*
const std::vector<std::string> Mode_Vector
{
   "end",
   "free", 
   "used" 
};

enum class Mode
{
   end,
   free,
   used
};
*/ 

const std::vector<std::string> Mode_Vector
{
   "free", 
   "used", 
   "end"
};

enum class Mode
{
   free,
   used,
   end 
};



public:
   //some aliases
   class Iterator;
   using value_type = Key;
   using key_type = Key;
   using reference = value_type &;
   using const_reference = const value_type &;
   using size_type = size_t;
   using difference_type = std::ptrdiff_t;
   using const_iterator = Iterator;
   using iterator = const_iterator;
   //using key_compare = std::less<key_type>;   // B+-Tree
   using key_equal = std::equal_to<key_type>; // Hashing
   using hasher = std::hash<key_type>;        // Hashing


//private:

	//instanzvariablen
	struct Bucket
	{
      key_type data;
      Bucket* next_ptr {nullptr};
      Mode mode {Mode::free};
	};
   private:
   Bucket* table {nullptr};
   size_type current_sz {0};
   size_type max_sz {0};
   float max_lf {0.69}; //nice

public:

   //konstruktoren

   //1ter konstruktor
   ADS_set():
   table {new Bucket [N+1]},
   current_sz {0},
   max_sz {N},
   max_lf {0.69} //nice
   {
      //Bucket* ptr {table + max_sz};
      //ptr->mode = Mode::end;            //aus irgendeinem fucking grund kann ich das mode bei N+1 nicht setzten aber es funktioneirt wenn ich in der enum klasse das mode
      table[max_sz].mode = Mode::end;     //welches ich will als erstes setzte
   }

   //2ter konstruktor
   ADS_set(std::initializer_list<key_type> ilist): //initializer_list is always const 4 some reason
   table {new Bucket [((ilist.size()+1)*2)+1]},   //table {new Bucket[N]};
   current_sz {0},
   max_sz {(ilist.size()+1)*2},
   max_lf {0.69}
   {
      for(const key_type* ilist_ptr {ilist.begin()}; ilist_ptr != ilist.end(); ++ilist_ptr) //for(auto ilist_ptr {ilist.begin()}; ilist_ptr != ilist.end(); ++ilist_ptr)?
      {
         //if(*ilist_ptr) == 0)
         //{
            add(*ilist_ptr);
         //}  
      }
      table[max_sz].mode = Mode::end;
   }

   //3ter konstruktor
   template<typename InputIt> ADS_set(InputIt first, InputIt last):
   table {new Bucket [N+1]},
   current_sz {0},
   max_sz {N},
   max_lf {0.69}
   {
      for(auto it {first}; it != last; ++it)
      {
         //if(count(*it) == 0)
         //{
            add(*it);
         //}
      }
      table[max_sz].mode = Mode::end;
   }

   //4ter konstruktor
   //this = other
   //also das this objekt bekommt das zeugs welches in other steht
   ADS_set(const ADS_set &other):
   table {new Bucket[other.max_sz + 1]},   //table {new Bucket[N]};
   current_sz {other.current_sz},
   max_sz {other.max_sz},
   max_lf {0.69}
   {

      Bucket* this_table_ptr {this->table};     //zeigt auf die table vom this objekt
      Bucket* this_list_ptr {this_table_ptr};   //zeigt auf die liste vom this objekt

      Bucket* other_table_ptr {other.table};    //zeigt auf die table vom other objekt
      Bucket* other_list_ptr {other_table_ptr}; //zeigt auf die liste vom other objekt

      for(size_type i {0}; i<other.max_sz; ++i)
      {
         do
         {
            this_list_ptr->data = other_list_ptr->data;
            if(other_list_ptr->next_ptr != nullptr)
            {
               this_list_ptr->mode = other_list_ptr->mode;
               Bucket* this_new_next_bucket {new Bucket}; //das ist das neue bucket
               this_list_ptr->next_ptr = this_new_next_bucket; //this_list_ptr->ptr zeigt auf das naechste bucket
               this_list_ptr = this_list_ptr->next_ptr;
               other_list_ptr = other_list_ptr->next_ptr;

            }
            else                                   //bin mir sehr sicher dass ich das else hier
            {                                      //und das was in der klammer von else ist wegmachen kann
               this_list_ptr->next_ptr = nullptr;
               this_list_ptr->mode = other_list_ptr->mode;
               other_list_ptr = other_list_ptr->next_ptr;
            }

         }
         while (other_list_ptr != nullptr);

         ++this_table_ptr;    //scahltet den pointer auf den naechsten eintrag in die table
         ++other_table_ptr;   //scahltet den pointer auf den naechsten eintrag in die table

         this_list_ptr = this_table_ptr;     //"resettet" den pointer zurrueck zur anfang der liste aber beim naechsten table eintrag
         other_list_ptr = other_table_ptr;   //"resettet" den pointer zurrueck zur anfang der liste aber beim naechsten table eintrag
      }

      this_table_ptr = (table + max_sz);
      this_table_ptr->mode = Mode::end;
      
   }

   //scuffed dekonstruktor. funktioniert jetzt
   ~ADS_set()
   {
      Bucket* table_ptr {this->table};
      Bucket* list_ptr {table_ptr};
      Bucket* next_list_ptr {table_ptr};
      bool first {true};

      for(size_type i {0}; i<max_sz; ++i)
      {
         first = true;
         do
         {
            if(list_ptr->mode == Mode::free)   //ich glaube dashier brauche ich actually nciht aber whatever
            {
               break;
            }
            next_list_ptr = list_ptr->next_ptr;
            if(first == true)
            {
               first = false;
            }
            else
            {
               delete list_ptr;
            }
            list_ptr = next_list_ptr;
         }
         while(list_ptr != nullptr);

         ++table_ptr;
         list_ptr = table_ptr;
         next_list_ptr = list_ptr;
      }
      delete [] table;
   }

   //methoden

   //wie der dekonstruktor, nur als methode
   void incinerate(Bucket* &table, const size_t& max_sz)
   {
      Bucket* table_ptr {table};
      Bucket* list_ptr {table_ptr};
      Bucket* next_list_ptr {table_ptr};
      bool first {true};

      for(size_type i {0}; i<max_sz; ++i)
      {
         first = true;
         do
         {
            if(list_ptr->mode == Mode::free)   //ich glaube dashier brauche ich actually nciht aber whatever
            {
               break;
            }
            next_list_ptr = list_ptr->next_ptr;
            if(first == true)
            {
               first = false;
            }
            else
            {
               delete list_ptr;
            }
            list_ptr = next_list_ptr;
         }
         while(list_ptr != nullptr);

         ++table_ptr;
         list_ptr = table_ptr;
         next_list_ptr = list_ptr;
      }
      delete [] table;
   }

   //add methode (von mir)
   //der job von der add methode ist es das uebergebene datum einzufuegen mittels der hashmethode und mehr platz zu reservieren falls nicht genug da ist
   void add(const key_type& data_to_insert)
   {
      //speicher reservieren
      if(static_cast<float>(current_sz)/static_cast<float>(max_sz) > max_lf || max_sz == 0)
      {
         this->reserve((max_sz+1)*2);
      }

      size_type index {hash(data_to_insert)}; //data wird gehasht um den index rauszufinden

      Bucket* bucket_ptr {this->table + index}; //zeigt auf den ersten bucket in der liste
      

      //falls der erste bucket direkt frei ist dann mach dashier
      if(bucket_ptr->mode == Mode::free)
      {
         //if(!(key_equal{}(bucket_ptr->data, data_to_insert)))
         //{
            bucket_ptr->data = data_to_insert;
            bucket_ptr->mode = Mode::used;
            current_sz = current_sz + 1;
         //}
      }
      else //falls der erste bucket nicht frei ist
      {
         while(bucket_ptr->mode == Mode::used) //solange der bucket schon verwendet wird,
         {                                      //schalte auf den naechsten
            if(key_equal{}(bucket_ptr->data, data_to_insert))
            {
               break;
            }
            if(bucket_ptr->next_ptr == nullptr) //falls der next_ptr auf null ziegt, dann fuege den neuen bucket hinzu
            {
               Bucket* bucket_to_insert {new Bucket};    //kreiere einen neuen bucket //muss dashier vor der if/else afrage?
               bucket_to_insert->next_ptr = nullptr;     //glaube das brauche ich gar nicht weil es per default auf nullptr zeigt aber whatever
               bucket_to_insert->data = data_to_insert;  //tu das datum welches hinzugefuegt wird in den neuen bucket
               bucket_to_insert->mode = Mode::used;            //einfach jetzt schon mal auf true setzen damit ich es spaeter nicht vergesse weil alzheimer
               bucket_ptr->next_ptr = bucket_to_insert;
               current_sz = current_sz + 1;
               break;
            }
            bucket_ptr = bucket_ptr->next_ptr;  //schalte aufs naechste bucket in der liste
         }

      }
   }

   //hashfunktion
   size_type hash(const key_type &data_to_be_hashed) const
   {
      return hasher{}(data_to_be_hashed) % max_sz; //return std::hash<key_type>{}(data_to_be_hashed) % max_sz;
   }                                                           //return hasher{}(data_to_be_hashed) % table_size;

   //reserviert speicher
   void reserve(size_type new_sz)
   {
      //falls der loadfactor (0.69 (nice)) nicht eingehlaten wird, dann wird die new_size so lange vergroessert bis der maximale loadfactor passt
      while(static_cast<float>(max_sz) / static_cast<float>(new_sz) > max_lf) //ich glaube das passt so
      {
         new_sz = (new_sz + 1) * 2;
      }

      Bucket* temp {new Bucket[this->max_sz]};  //neues temporaeres array welches so gross ist wie das originale (also es ist so gross wie max_sz)
      Bucket* temp_table_ptr {temp};            //pointer zum temporaeren array (wird nur auf die array-table zeigen)
      Bucket* temp_list_ptr {temp_table_ptr};   //pointer zum temporaeren array (wird spaeter auf die liste zeigen)

      Bucket* this_table_ptr {this->table};     //pointer zum temporaeren array (wird nur auf die array-table zeigen)
      Bucket* this_list_ptr {this_table_ptr};   //pointer zum temporaeren array (wird spaeter auf die liste zeigen)

      for(size_type i{0}; i<max_sz; ++i)
      {
         while(this_list_ptr != nullptr)
         {
            temp_list_ptr->data = this_list_ptr->data;
            temp_list_ptr->mode = this_list_ptr->mode;

            if(this_list_ptr->next_ptr != nullptr)
            {
               Bucket* bucket_to_insert {new Bucket};
               temp_list_ptr->next_ptr = bucket_to_insert;
            }

            this_list_ptr = this_list_ptr->next_ptr; //schalte auf den naechsten eintrag in der liste
            temp_list_ptr = temp_list_ptr->next_ptr;  //schalte auf den naechsten eintrag in der liste
         }

         ++temp_table_ptr;
         temp_list_ptr = temp_table_ptr;
         ++this_table_ptr;
         this_list_ptr = this_table_ptr;
      }


      //loescht die alte this table
      this_table_ptr = this->table;
      this_list_ptr = this_table_ptr;
      Bucket* this_next_list_ptr {this_table_ptr};
      bool first {true};

      for(size_type i {0}; i < this->max_sz; ++i)
      {
         first = true;
         do
         {
            if(this_list_ptr->mode == Mode::free)
            {
               break;
            }
            this_next_list_ptr = this_list_ptr->next_ptr;
            if(first == true)
            {
               first = false;
            }
            else
            {
               delete this_list_ptr;
            }
            this_list_ptr = this_next_list_ptr;
         }
         while(this_list_ptr != nullptr);

         ++this_table_ptr;
         this_list_ptr = this_table_ptr;
         this_next_list_ptr = this_list_ptr;
      }
      delete [] table;


      table = new Bucket [new_sz+1]; //table hat jetzt die neue size

      temp_table_ptr = temp;            //pointer zum temporaeren array (wird nur auf die array-table zeigen)
      temp_list_ptr = temp_table_ptr;   //pointer zum temporaeren array (wird spaeter auf die liste zeigen)
      current_sz = 0;

      size_type temp_max_sz {max_sz};
      max_sz = new_sz;                       //max_sz now has the size of new_sz
      for(size_type i{0}; i<temp_max_sz; ++i)
      {
         if(temp_list_ptr->mode == Mode::used)
         {
            do
            {
               add(temp_list_ptr->data);
               temp_list_ptr = temp_list_ptr->next_ptr;
            }
            while(temp_list_ptr != nullptr);
         }

         ++temp_table_ptr;
         temp_list_ptr = temp_table_ptr;

      }
      this_table_ptr = table+max_sz;      //sets the table pointer to the last index in the table
      this_table_ptr->mode = Mode::end;   //sets the mode to end

      //loescht die temp table
      temp_table_ptr = temp;
      temp_list_ptr = temp_table_ptr;
      Bucket* temp_next_list_ptr {temp_table_ptr};

      for(size_type i {0}; i<temp_max_sz; ++i)
      {
         first = true;
         do
         {
            if(temp_list_ptr->mode == Mode::free)
            {
               break;
            }
            temp_next_list_ptr = temp_list_ptr->next_ptr;
            if(first == true)
            {
               first = false;
            }
            else
            {
               delete temp_list_ptr;
            }
            temp_list_ptr = temp_next_list_ptr;
         }
         while(temp_list_ptr != nullptr);

         ++temp_table_ptr;
         temp_list_ptr = temp_table_ptr;
         temp_next_list_ptr = temp_list_ptr;
      }
      delete [] temp;


   }



/*
   //kopierzuweisungsoperator
   //also basically das gleiche wie 4ter konstruktor
   //this = other
   ADS_set &operator=(const ADS_set &other)
   {
      this->incinerate(this->table, this->max_sz);
      //this->clear;
      //delete [] table;
      this->table = new Bucket [other.max_sz+1];

      Bucket* this_table_ptr {this->table};     //zeigt auf die table vom this objekt
      Bucket* this_list_ptr {this_table_ptr};   //zeigt auf die liste vom this objekt

      Bucket* other_table_ptr {other.table};    //zeigt auf die table vom other objekt
      Bucket* other_list_ptr {other_table_ptr}; //zeigt auf die liste vom other objekt

      for(size_type i {0}; i<other.max_sz; ++i)
      {
         do
         {
            this_list_ptr->data = other_list_ptr->data;
            if(other_list_ptr->next_ptr != nullptr)
            {
               this_list_ptr->mode = other_list_ptr->mode;
               Bucket* this_new_next_bucket {new Bucket}; //das ist das neue bucket
               this_list_ptr->next_ptr = this_new_next_bucket; //this_list_ptr->ptr zeigt auf das naechste bucket
               this_list_ptr = this_list_ptr->next_ptr;
               other_list_ptr = other_list_ptr->next_ptr;

            }
            else                                   //bin mir sehr sicher dass ich das else hier
            {                                      //und das was in der klammer von else ist wegmachen kann
               this_list_ptr->next_ptr = nullptr;
               this_list_ptr->mode = other_list_ptr->mode;
               other_list_ptr = other_list_ptr->next_ptr;
            }



         }
         while (other_list_ptr != nullptr);

         ++this_table_ptr;    //scahltet den pointer auf den naechsten eintrag in die table
         ++other_table_ptr;   //scahltet den pointer auf den naechsten eintrag in die table

         this_list_ptr = this_table_ptr;     //"resettet" den pointer zurrueck zur anfang der liste aber beim naechsten table eintrag
         other_list_ptr = other_table_ptr;   //"resettet" den pointer zurrueck zur anfang der liste aber beim naechsten table eintrag
      }
      this->current_sz = other.current_sz;
      this->max_sz = other.max_sz;
      this->max_lf = other.max_lf;
      
      this_table_ptr = (table + max_sz);
      this_table_ptr->mode = Mode::end;
      
      return *this;
   }
*/

   //copy and swap goes brr
	ADS_set &operator=(const ADS_set &other)
	{
      ADS_set tmp{other};
      this->swap(tmp);
      return *this;
	}

   //der inhalt des containers wird durch die elemente aus ilist ersetzt
   ADS_set &operator=(std::initializer_list<key_type> ilist)
   {
      if((static_cast<float>(ilist.size())/static_cast<float>(this->max_sz)) > max_lf) //falls die groesse der ilist den loadfactor von max_lf
      {                                                                                //ueberschreiten wuerde, dann muss mehr speicher reserved werden
         this->reserve((ilist.size()+1)*2);
      }
      
      //
      this->clear(); //WEISS NICHT OB ICH ES CLEAREN SOLL ODER NICHT
      //             //JA SOLLST DU DU RETARD

      for(const key_type* ilist_ptr {ilist.begin()}; ilist_ptr != ilist.end(); ++ilist_ptr)
      {
         //if(*ilist_ptr) == 0)
         //{
            add(*ilist_ptr);
         //}
         
      }

      return *this;
   }

   //returnt die current_sz (also wie viele eintraege die tabelle hat (nicht max_sz!))
   size_type size() const  //size_type size() const
   {
      return current_sz;
   }

   //returnt true falls current_sz == 0. also returne 0 falls die tabelle leer ist (nicht max_sz!)
   bool empty() const
   {
      if(current_sz == 0)
      {
         return true;
      }
      return false;
   }

   template<typename InputIt> void insert(InputIt first, InputIt last)
   {
      for(auto it{first}; it != last; ++it)
      {
         //if(count(*it) == 0)
         //{
            add(*it);
         //}
         
      }
   }

   //returnt 1 falls das datum "data" gefunden wird.
   //falls es nicht gefunden wird, returne 0
   size_type count(const key_type& data_to_be_searched) const
   {
      size_type index {hash(data_to_be_searched)};
      Bucket* list_ptr {table + index};

      if(list_ptr->mode == Mode::used)
      {
         do
         {
            if(key_equal{}(list_ptr->data, data_to_be_searched))
            {
               return 1;   //falls data gefunden wird, returne 1;
            }
            list_ptr = list_ptr->next_ptr;
         }
         while(list_ptr != nullptr);
      }

      return 0;
   }

   Bucket* locate(const key_type& data_to_be_searched)
   {
      size_type index {hash(data_to_be_searched)};
      Bucket* list_ptr {table + index};

      if(list_ptr->mode == Mode::used)
      {
         do
         {
            if(key_equal{}(list_ptr->data, data_to_be_searched))
            {
               return list_ptr;   //falls data gefunden wird, returne list_ptr;
            }
            list_ptr = list_ptr->next_ptr;
         }
         while(list_ptr != nullptr);
      }
      return nullptr;
   }

   //fuegt elemente aus ilist ein
   void insert(std::initializer_list<key_type> ilist)   //void insert(std::initializer_list<key_type> ilist);
   {
      for(const key_type* ilist_ptr {ilist.begin()}; ilist_ptr != ilist.end(); ++ilist_ptr) //for(const key_type* ilist_ptr {ilist.begin()}; ilist_ptr != ilist.end(); ++ilist_ptr)
      {
         //if(count(*ilist_ptr) == 0)
         //{
            add(*ilist_ptr);
         //}
         
      }
   }

   //ph2
   //entfernt alle elemente aus dem container. also es entfernt alle linked lists und das array, aber nicht den tablepointer selbst
   //(nehme ich mal an. ka obs stimmt. es steht nichts auf cewebs)
   void clear()
   {
      Bucket* table_ptr {this->table};
      Bucket* list_ptr {table_ptr};
      Bucket* next_list_ptr {table_ptr};
      bool first {true};

      //loescht alle linked lists
      for(size_type i {0}; i<max_sz; ++i)
      {
         first = true;
         do
         {
            if(list_ptr->mode == Mode::free)   //ich glaube dashier brauche ich actually nicht aber whatever
            {
               break;
            }
            next_list_ptr = list_ptr->next_ptr;
            if(first == true)
            {
               first = false;
            }
            else
            {
               delete list_ptr;
            }
            list_ptr = next_list_ptr;
         }
         while(list_ptr != nullptr);

         ++table_ptr;
         list_ptr = table_ptr;
         next_list_ptr = list_ptr;
      }

      //loescht das array
      //ich weiss nicht ob ich einfach dashier machen darf. aber laut der angabe auf cewebs "Entfernt alle Elemente aus dem Container." sollte es passen
      delete [] table;
      table = new Bucket [N+1];
      table_ptr = table+N;
      table_ptr->mode = Mode::end;
      current_sz = 0;
      max_sz = N;

   }

   //phase 2
   //loescht den bucket welcher das datum "data_to_erase" beinhaltet und returnt 1 falls es was geloscht hat und 0 falls es nicht geloescht hat
   size_type erase(const key_type &data_to_erase)
   {
      size_type index {hash(data_to_erase)};
      Bucket* previous_list_ptr{this->table + index};
      Bucket* list_ptr {nullptr};
      Bucket* next_list_ptr{nullptr};
      

      if(previous_list_ptr->mode == Mode::used) //falls die liste am index am anfang direkt leer ist kann abgebrochen werden
      {
         if(key_equal{}(previous_list_ptr->data, data_to_erase)) //falls der bucket welcher in der liste an erster stelle ist das datum beinhaltet welches geloescht werden soll
         {
            if(previous_list_ptr->next_ptr == nullptr)
            {
               previous_list_ptr->mode = Mode::free;
               current_sz = current_sz - 1;
               return 1;
            }
            //falls nach dem geloeschten ersten bucket es noch weitere buckets gibt
            //dann muessen alle buckets nach hinten gezogen werden
            list_ptr = previous_list_ptr->next_ptr;
            if(list_ptr->next_ptr == nullptr)
            {
               previous_list_ptr->data = list_ptr->data;
               previous_list_ptr->next_ptr = nullptr;
               delete list_ptr;
               current_sz = current_sz - 1;
               return 1;
            }
            do
            {
               previous_list_ptr->data = list_ptr->data;
               previous_list_ptr = previous_list_ptr->next_ptr;
               list_ptr = list_ptr->next_ptr;
            }
            while(list_ptr->next_ptr != nullptr);
            previous_list_ptr->data = list_ptr->data;
            previous_list_ptr->next_ptr = nullptr;
            delete list_ptr;
            current_sz = current_sz - 1;
            return 1;
         }
         
         if(previous_list_ptr->next_ptr == nullptr)
         {
            return 0;
         }

         list_ptr = previous_list_ptr->next_ptr; //list_ptr zeigt auf den 2ten eintrag
         next_list_ptr = list_ptr->next_ptr; //next_list_ptr zeigt auf den 3ten eintrag
         do
         {
            if(key_equal{}(list_ptr->data, data_to_erase))
            {
               delete list_ptr;
               previous_list_ptr->next_ptr = next_list_ptr;
               current_sz = current_sz - 1;
               return 1;
            }
            previous_list_ptr = previous_list_ptr->next_ptr;
            list_ptr = list_ptr->next_ptr;
            if(next_list_ptr != nullptr)
            {
               next_list_ptr = next_list_ptr->next_ptr;
            }
         }
         while(list_ptr != nullptr);
      }
      return 0;
   }

   //phase 2
   //vertauscht die elemente von "this" mit den elementen von "other"
   //swap(this,other);
   void swap(ADS_set &other)
   {
      //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA SWAP WIRD VERWENDET" << std::endl;
      
      //DER VORGANG: (checks out)
      //alles von other kommt in temp
      //alles von this kommt in other
      //alles von temp kommt in this
      //loesche temp

      //NEUE VORGANGSWEISE:
      //temp zeigt auf other
      //other zeigt auf this
      //this zeigt auf temp
      
      //temp zeigt auf other
      Bucket* temp_table_pointer = other.table;
      size_t temp_max_sz {other.max_sz};
      size_t temp_current_sz {other.current_sz};
      float temp_max_lf {other.max_lf};

      //other zeigt auf this
      other.table = this->table;
      other.max_sz = this->max_sz;
      other.current_sz = this->current_sz;
      other.max_lf = this->max_lf;

      //this zeigt auf temp
      this->table = temp_table_pointer;
      this->max_sz = temp_max_sz;
      this->current_sz = temp_current_sz;
      this->max_lf = temp_max_lf;

   }


   //ph2
   //NEUES OPERATOR==
   //basically von polaschek geflaucht
   friend bool operator==(const ADS_set &lhs, const ADS_set &rhs)
   {
      if (lhs.current_sz != rhs.current_sz) 
      {
         return false;
      }
      for (const auto &k: lhs) 
      {
         if (!rhs.count(k))
         {
            return false;
         }
      } 
      return true;
   }


   //ph2
   friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs)
   {
      if(lhs == rhs) //ka ob dashier funktioniert. hopium schon. will das ganze nicht nochmal machen fuer !=
      {
         return false;
      }                 //bin mir nicht sicher ob ich hier false und true vertauschen muss. aber ich glaube das passt so
      return true;
   }

   //phase 2
   //returns iterator which points to the bucket which contains the data
   //if the data is not found, return iterator which points to end
   iterator find(const key_type &data_to_find) const
   {     
      size_type index {hash(data_to_find)};

      Bucket* table_ptr {table + index};
      Bucket* list_ptr {table_ptr};

      if(list_ptr->mode == Mode::used)
      {
         do
         {
            if(key_equal{}(list_ptr->data, data_to_find))
            {
               return iterator{table_ptr, list_ptr};   //falls data gefunden wird, returne list_ptr;
            }
            list_ptr = list_ptr->next_ptr;
         }
         while(list_ptr != nullptr);
      }
      return iterator{end()};
   }
   
   //phase 2
   //return true if data_to_be_inserted is inserted, false else
   std::pair<iterator,bool> insert(const key_type &data_to_be_inserted)
   {
      //reserve memory
      if(static_cast<float>(current_sz)/static_cast<float>(max_sz) > max_lf || max_sz == 0)
      {
         this->reserve((max_sz+1)*2);
      }

      size_type index {hash(data_to_be_inserted)};
      Bucket* table_ptr {table + index};
      Bucket* list_ptr {table_ptr};

      if(list_ptr->mode == Mode::free) //if the first bucket is free
      {
         list_ptr->data = data_to_be_inserted;
         list_ptr->mode = Mode::used;
         current_sz = current_sz + 1;
         //std::cout << "added: " << data_to_be_inserted << " cuz first free" << std::endl; //DEBUG
         return {iterator{table_ptr, list_ptr},true}; //return iterator{table_ptr,list_ptr,true};
      }
      
      //if the first bucket is not free
      while(list_ptr->mode == Mode::used)
      {
         if(key_equal{}(list_ptr->data, data_to_be_inserted)) //if data_to_insert is already found in the list
         {
            //std::cout << "not added " << data_to_be_inserted << " cuz found" << std::endl; //DEBUG
            return {iterator{table_ptr, list_ptr},false};   //return pointer to it and return false;
         }
         if(list_ptr->next_ptr == nullptr)
         {
            break;
         }
         list_ptr = list_ptr->next_ptr;
      }

      current_sz = current_sz + 1;
      Bucket* bucket_to_insert {new Bucket};
      bucket_to_insert->data = data_to_be_inserted;
      bucket_to_insert->mode = Mode::used;
      bucket_to_insert->next_ptr = nullptr;
      list_ptr->next_ptr = bucket_to_insert;
      list_ptr = list_ptr->next_ptr;
      //std::cout << "added: " << list_ptr->data << " cuz end of list" << std::endl; //DEBUG
      //std::cout << std::endl << "table_ptr->data: " << table_ptr->data << std::endl << "list_ptr->data: " << list_ptr->data << std::endl; //DEBUG
      return {iterator{table_ptr, list_ptr},true};

   }

   //phase 2
   const_iterator begin() const
   {
      return const_iterator{this->table, this->table};
   }

   //phase 2
   const_iterator end() const
   {
      return const_iterator{this->table + this->max_sz, this->table + this->max_sz};
   }


   //NEW DEFAULT OUTPUT
   void dump(std::ostream &o = std::cerr) const
   {
      o << "table_size (also known as max_sz) = " << max_sz << std::endl << "current_size = " << current_sz << std::endl << std::endl;

      Bucket* table_ptr {table};
      Bucket* list_ptr {table_ptr};

      for(size_type i {0}; i<max_sz+1; ++i)
      {
         o << i << ": ";
         
         if(list_ptr->mode == Mode::used)
         {
            do
            {
               o << list_ptr->data << " ";
               list_ptr = list_ptr->next_ptr;

            }
            while(list_ptr != nullptr);
            
         }
         else
         {
            o << Mode_Vector.at(static_cast<size_t>(table_ptr->mode));
            list_ptr = list_ptr->next_ptr;
         }

         o << std::endl;
         ++table_ptr;
         list_ptr = table_ptr;
      }

   }















   //CLASS ITERATOR
   //template <typename Key, size_t N>
   class Iterator
   {
      //instance variables
      Bucket* table_iterator;
      Bucket* list_iterator;
      
public:
      using value_type = Key;
      using difference_type = std::ptrdiff_t;
      using reference = const value_type &;
      using pointer = const value_type *;
      using iterator_category = std::forward_iterator_tag;

      //constructor
      explicit Iterator(Bucket* table_ptr = nullptr, Bucket* list_ptr = nullptr): 
      table_iterator{table_ptr}, 
      list_iterator{list_ptr}
      {
         if(table_iterator != nullptr)                //if table_iterator is pointing to the table
         {
            /*
            //dont need this stuff. just call skip_iterator(); instead
            if(table_ptr->mode == Mode::free)
            {
               while(table_iterator->mode == Mode::free) //while the mode of table_iterator is Mode::free
               {                                         //point to the next entry
                  ++table_iterator;
               }
               list_iterator = table_iterator;           //list_iterator points now to table_iterator because if you didnt give a valid table_ptr, you 4 sure didnt give a valid list_ptr
            }
            */

            if(table_iterator->mode == Mode::free)
            {
               //skip_iterator();   //dont need
               skip_table();        //skip_table(); is enough
            }
         }
         if(list_iterator == nullptr)
         {
            list_iterator = table_iterator;
         }

      }


      //methods

      Iterator &operator++()
      {
         skip_iterator();
         return *this;
      }

      Iterator operator++(int)
      {
         auto iterator_to_return {*this};
         ++*this;
         return iterator_to_return;
      }

      //skips to the next entry in the table which has mode::used
      void skip_table()
      {
         while(table_iterator->mode == Mode::free) //while the mode of table_iterator is Mode::free
         {                                         //point to the next entry
            ++table_iterator;
         }
         list_iterator = table_iterator;           //list_iterator points now to table_iterator because if you didnt give a valid table_ptr, you 4 sure didnt give a valid list_ptr
      }

      //skips to the next bucket which is used
      Iterator skip_iterator()
      {
         if(list_iterator->mode == Mode::end)
         {
            throw std::runtime_error("cant increment because list_iterator is already at end. thrown from 'skip_iterator'");
         }
         if(list_iterator->next_ptr != nullptr) //if the next bucket in the list exists
         {
            list_iterator = list_iterator->next_ptr;  //list_iterator now points to next bucket
            return *this;
         }
         else                                //if next bucket in the list does not exist
         {
            ++table_iterator;                //goes to the next entry in the table cuz u need to manually so that table_iterator->mode is no longer Mode::free
            skip_table();                    //before it goes into the while(table_iterator->mode == Mode::free) loop which is in skip_table();
            //list_iterator = table_iterator; //dont need this cuz skip_table already does this
         }
         return *this;
      }

      reference operator*() const
      {
         if(list_iterator->mode == Mode::end)
         {
            throw std::runtime_error("cant get data because list_iterator is already at end. thrown from 'reference operator*() const'");
         }
         return list_iterator->data;
      }

      pointer operator->() const
      {
         if(list_iterator->mode == Mode::end)
         {
            throw std::runtime_error("cant get data because list_iterator is already at end. thrown from 'pointer operator->() const'");
         }
         return &list_iterator->data;
      }

      friend bool operator==(const Iterator&lhs, const Iterator&rhs)
      {
         if(lhs.list_iterator == rhs.list_iterator && lhs.table_iterator == rhs.table_iterator)
         {
            return true;
         }
         return false;
      }

      friend bool operator!=(const Iterator&lhs, const Iterator&rhs)
      {
         if(lhs == rhs) //return !(lhs == rhs)
         {
            return false;
         }
         return true;
      }

   };

};

//anscheinend brauche ich diesen muell damit swap mein eigenes swap verwendet und nicht das std::swap
template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs)
{
   lhs.swap(rhs);
}



#endif //ADS_SET_H
