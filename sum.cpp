#include <functional>
#include <iostream>
#include<vector>
#include <algorithm>



int main()
{
   /*int a = 5;
   int b = 1;
   std::cout << std::plus<int>{}(a, b) << '\n';*/
   std::vector<int> a={2,0,1,5,0};//looks like this: 2,0,1,5,0
   std::vector<int> b={0,0,1,3,5};//looks like this: 0,0,1,3,5
   
   std::transform (a.begin(), a.end(), b.begin(), a.begin(), std::plus<int>());
   
 
}

