#include <thread>
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <mutex>

//CRITICAL SECTION
//una critical section �s un tros de codi que accedeix a un recurs que �s compartit (variables globals, array d'objectes...)
//les seccions cr�tiques s'han de protegir am un objecte anomenat "mutex". Significa mutual exclusion. �s com una caixa amb 
//la qual envoltem les seccions cr�tiques perqu� dos threads a la vegada no puguin entrar al mateix tros de codi.
//S'ha d'intentar bloquejar la menor part de codi possible. A sota bloquegem cada iteraci� del for per separat. El lock �s una 
//inst�ncia d'un objecte que s'elimina sol al for, per� hi ha la possibilitat de fer servir unlock

//EVENTS
//Serveixen per sinronitzar threads
//l'objecte s'anomena "condition_variable"

int counter = 0; // Global var
std::mutex mutex;
void increment(int iterations) {
	for (int i = 0; i < iterations; ++i) {
		std::unique_lock<std::mutex> lock(mutex);
		counter++;
	}
}
int main() {
	std::thread t1(increment, 100000); // Execute 'increment' in a thread
	std::thread t2(increment, 100000); // Execute 'increment' in a thread
	t1.join(); // Wait for t1
	t2.join(); // Wait for t2
	std::cout << "Counter = " << counter << std::endl;
	return 0;
}