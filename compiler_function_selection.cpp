// Field Selection Error

//// class Snippet {
//// public:
////     double func(double n) {
////         return n;
////     }
//// private:
////     int func(int n) {
////         return n;
////     }
//// };
//// 
//// int main() {
////     Snippet snippet;
//// 
////     // XXX:
////     // 1. name selection: func(double) and func(int)
////     // 2. overload selection: func(int)
////     // 3. field selection: func(int) is private, Failed to be accessed
//// 
////     snippet.func(7); 
////     return 0;
//// }


// Overload Selection Error
//// class Snippet {
//// public:
////     double func(double n) {
////         return n;
////     }
//// private:
////     unsigned func(unsigned n) {
////         return n;
////     }
//// };
//// 
//// int main() {
////     Snippet snippet;
//// 
////     // XXX:
////     // 1. name selection: func(double n) and func(int)
////     // 2. override selection: failed to select one function
////     snippet.func(7);
////     return 0;
//// }

// Name Selection Error

//// #include <string>
//// 
//// int func(int n) {
////     return n;
//// }
//// 
//// class Snippet {
//// public:
////     void test() {
////         func(7);
////     }
//// private:
////     std::string func(std::string n) {
////         return n;
////     }
//// };
//// 
//// int main() {
////     Snippet snippet;
//// 
////     // XXX:
////     // 1. name selection: func(std::string). !!! if found one function in current field, do not find again, so global function: func(int) is ignored
////     // 2. override selection: failed to override
////     snippet.test();
////     return 0;
//// }