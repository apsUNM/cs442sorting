#include <bits/stdc++.h>

using namespace std;

int sPartition(float arr[], int low, int high) {
    float pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void sQuicksort(float arr[], int low, int high) {
    if (low < high) {
        int pi = sPartition(arr, low, high);
        sQuicksort(arr, low, pi - 1);
        sQuicksort(arr, pi + 1, high);
    }
}