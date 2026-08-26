# times tables

func multiply a, b ->
{
	let total = 0.

	for let i = 0. i < a. i = i + 1.
	{
		total = total + b.
	}

	return total.
}

func sixSeven times ->
{
	let ss = "SIX SEVEN".

	for let i = 0. i < times. i = i + 1.
	{
		print ss.
		ss = ss + "N".
	}
}

for let i = 1. i <= 12. i = i + 1.
{
	for let j = 1. j <= 12. j = j + 1.
	{
		if i == 6 and j == 7 then
		{
			sixSeven(multiply(6, 7)).
		}
		print i + " x " + j + " = " + multiply(i, j).
	}
}
