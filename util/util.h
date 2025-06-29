int length(const char* str) {
    int length = 0;
    for (;str[length++];);
    return length;
}