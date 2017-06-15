# Sorting Visualization

A miniproject to learn more about sorting algorithms, as well as OpenGL.

## Todo

Organize all of the sorting algorithms into a library

Highlight the elements currently being swapped/compared

Add sound

## Bubble sort

An algorithm that cycles through the list, comparing pairs of adjacent elements and swapping them if out of order. It stops cycling after a full cycle without swaps.

It's known as bubble sort because the higher values tend to "bubble" towards the end of the list.

## Insertion sort

An algorithm that goes through the list and places each element it finds into order in relation to the previous elements.

## Radix sort

Radix sort goes though the entire list and classifies elements based on their bits. In the case implemented here, each cycle checks 2 bits, so it is in base 4, starting from the least significant digits and going up. By the end the elements are sorted.

Radix sort is a non-comparison sort, because it doesn't perform comparisons between elements of the list.

## Bogo sort

Also known as stupid sort, it check if the list is ordered, and if not, randomly shuffles it and checks again.

Because of its random nature, it's impossible to determine whether or not bogo sort will ever reach the solution for lists with more the 1 element.

## Heap sort

This algorithm organizes the list as a binary max heap, a hierarchical data structure in which the parent nodes are always greater than its children. After the heap has been created, the root element is placed at the end of the unsorted list(as it's guaranteed to be the greatest element), and the remainder of the list is readjusted to form a heap once again. It repeats this process until the list is sorted.

## Cocktail shaker sort

An algorithm similar to bubble sort, that cycles in both directions.
