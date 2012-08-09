module util;

int saneMod(int x, int y) {
	int result=x%y;
	if (result<0) result+=y;
	return result;
}

int divDown(int x, int y) {
	if (x<0) return ((x+1)/y)-1;
	return x/y;
}

int divUp(int x, int y) {
	if (x>0) return ((x-1)/y)+1;
	return x/y;
}
