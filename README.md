# Multithreaded-Primes-Sieve

This project implements the segmented sieve of Eratosthenes to find prime numbers having two or more digits that are ‘3’. Unlike the standard sieve of Eratosthenes, that operates over the range [2, b], the segmented version can start at an arbitrary lower bound a, working over the range [a, b]. Since a wide range of values can be programmed in segments, this algorithm lends itself nicely to a multithreaded approach.
