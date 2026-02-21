void exgcd(int a, int b, int* k, int* l)
{
	int	aa = a, bb = b, q, k1 = 1, l1 = 0, k2 = 0, l2 = -1;
	while (1)
	{
		q = aa / bb;
		int	kk = k1 - q * k2, ll = l1 - q * l2;
		k1 = k2, l1 = l2;
		k2 = kk, l2 = ll;
	};
	if (k2 < 0)	k2 += b, l2 += a;
	* k = k2, * l = l2;
}