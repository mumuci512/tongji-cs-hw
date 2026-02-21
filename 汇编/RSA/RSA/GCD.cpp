void gcd(int a, int b, int* c)
{
	int	aa = a, bb = b, rr;
	while ((rr = aa % bb) != 0)
	{
		aa = bb;
		bb = rr;
	}
	*c = bb;	//最大公约数
}