def egcd(a, b):
    if a == 0:
        return (b, 0, 1)
    else:
        g, y, x = egcd(b % a, a)
        return (g, x - (b // a) * y, y)

def modinv(a, m):
    g, x, y = egcd(a, m)
    if g != 1:
        raise Exception('modular inverse does not exist')
    else:
        return x % m

print "p_prime:", modinv(2**256-115792089210356248762697446949407573530086143415290314195533631308867097853951, 2**256), "\n"

print "r_2:", (2**512)%115792089210356248762697446949407573530086143415290314195533631308867097853951, "\n"
