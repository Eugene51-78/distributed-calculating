int ltime;

int get_lamport_time() {
    return ltime;
}

void increase_lamp_time(int time) {
    if (ltime < time)
        ltime = ++time;
    else ++ltime;
}
