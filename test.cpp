#include <iostream>
#include <cstdlib>
#include <bitset>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <Shlwapi.h>
#include <VersionHelpers.h>
#include <Windows.h>

double MakeTime(FILETIME const& kernel_time, FILETIME const& user_time) {
  ULARGE_INTEGER kernel;
  ULARGE_INTEGER user;
  kernel.HighPart = kernel_time.dwHighDateTime;
  kernel.LowPart = kernel_time.dwLowDateTime;
  user.HighPart = user_time.dwHighDateTime;
  user.LowPart = user_time.dwLowDateTime;
  std::cout << "Kernel: " << kernel.QuadPart << std::endl;
  std::cout << "User: " << user.QuadPart << std::endl;
  return (static_cast<double>(kernel.QuadPart) +
          static_cast<double>(user.QuadPart)) *
         1e-7;
}

double ProcessCPUUsage() {
  HANDLE proc = GetCurrentProcess();
  FILETIME creation_time;
  FILETIME exit_time;
  FILETIME kernel_time;
  FILETIME user_time;
  if (GetProcessTimes(proc, &creation_time, &exit_time, &kernel_time,
                      &user_time))
    return MakeTime(kernel_time, user_time);
  std::abort();
}


template<typename T>
void PrintBinRep(const char* Name, const T& tmp)
{
    using RawT = typename std::remove_volatile<T>::type;
    const RawT& a = const_cast<const RawT&>(tmp);
    const unsigned char* beg = reinterpret_cast<const unsigned char*>(&a);
    const unsigned char* end = beg + sizeof(a);
    std::cout << Name << ": ";
    while (end != beg)
      std::cout << std::bitset<8>(*(--end)) << ' ';

    //while(beg != end)
    //    std::cout << std::bitset<CHAR_BIT>(*beg++) << ' ';
    std::cout << std::endl;
}


int main() {
	#if 0
	unsigned long long value1 = 1;
	unsigned long long value2 = 1;
	double d = (static_cast<double>(value1) + static_cast<double>(value2)) * 1e-7;
	volatile double otherd = d;
	double other = otherd - d;
	printf("otherd: %f, d: %f, other: %f\n", otherd, d, other);
	assert(other >= 0);
  #endif
  for (int i=0; i < 5; ++i) {
    double d1 = ProcessCPUUsage();
    double d2 = ProcessCPUUsage();
    double d3 = (d2 - d1);
    PrintBinRep("d1", d1);
    PrintBinRep("d2", d2);
    PrintBinRep("d3", d3);
    assert(!(d1 < 0));
    assert(!(d2 < 0));
    assert(!(d3 < 0));
  }
}
