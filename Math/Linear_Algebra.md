## Dot products and duality

Traditionally, dot products or something that's introduced really early on in a linear algebra
course typically right at the start.
it might seem strange that I push them back this far in the series.
I did this because there's a standard way to introduce the topic 
which requires nothing more than a basic understanding of vectors,
but a fuller understanding of the role the dot products play in math, 
can only really be found under the light of linear transformations.

Before that, though, let me just briefly cover the standard way that products are introduced.
Which I'm assuming is at least partially review for a number of viewers.
Numerically, if you have two vectors of the same dimension;
to list of numbers with the same length,
taking their dot product, means, pairing up all of the coordinates, multiplying those pairs together, and adding the result.

So the vector [1, 2] dotted with [3, 4],
would be 1 x 3 + 2 x 4.
The vector [6, 2, 8, 3] dotted with [1, 8, 5, 3] would be:
6 x 1 + 2 x 8 + 8 x 5 + 3 x 3.

Luckily, this computation has a really nice geometric interpretation.
To think about the dot product between two vectors v and w,
imagine projecting w onto the line that passes through the origin and the tip of v.
Multiplying the length of this projection by the length of v, you have the dot product v・w.

Except when this projection of w is pointing in the opposite direction from v,
that dot product will actually be negative.
So when two vectors are generally pointing in the same direction,
their dot product is positive.
When they're perpendicular, meaning,
the projection of one onto the other is the 0 vector,the dot product is 0.
And if they're pointing generally the opposite direction, their dot product is negative.

Now, this interpretation is weirdly asymmetric,
it treats the two vectors very differently,
so when I first learned this, I was surprised that order doesn't matter.
You could instead project v onto w;
multiply the length of the projected v by the length of w
and get the same result.
I mean, doesn't that feel like a really different process?

Here's the intuition for why order doesn't matter:
if v and w happened to have the same length,
we could leverage some symmetry.
Since projecting w onto v
then multiplying the length of that projection by the length of v,
is a complete mirror image of projecting v onto w then multiplying the length of that
projection by the length of w.
Now, if you “scale” one of them, say v by some constant like 2,
so that they don't have equal length,
the symmetry is broken.
But let's think through how to interpret the dot product between this new vector 2v and w.

If you think of w is getting projected onto v
then the dot product 2v・w will be exactly twice the dot product v・w.
This is because when you “scale” v by 2,
it doesn't change the length of the projection of w
but it doubles the length of the vector that you're projecting onto.
But, on the other hand, let's say you're thinking about v getting projected onto w.
Well, in that case, 
the length of the projection is the thing to get “scaled” when we multiply v by 2.
The length of the vector that you're projecting onto stays constant.
So the overall effect is still to just double the dot product.
So, even though symmetry is broken in this case,
the effect that this “scaling” has on the value of the dot product, is the same
under both interpretations.

There's also one other big question that confused me when I first learned this stuff:
Why on earth does this numerical process of matching coordinates, multiplying pairs and
adding them together, have anything to do with projection?

Well, to give a satisfactory answer,
and also to do full justice to the significance of the dot product,
we need to unearth something a little bit deeper going on here
which often goes by the name "duality".

But, before getting into that,
I need to spend some time talking about linear transformations
from multiple dimensions to one dimension
which is just the number line.

These are functions that take in a 2D vector and spit out some number.
But linear transformations are, of course,
much more restricted than your run-of-the-mill function with a 2D input and a 1D output.
As with transformations in higher dimensions,
like the ones I talked about in chapter 3,
there are some formal properties that make these functions linear.
But I'm going to purposely ignore those here so as to not distract from our end goal,
and instead focus on a certain visual property that's equivalent to all the formal stuff.

If you take a line of evenly spaced dots and apply a transformation,
a linear transformation will keep those dots evenly spaced,
once they land in the output space, which is the number line.
Otherwise, if there's some line of dots that gets unevenly spaced
then your transformation is not linear.

As with the cases we've seen before,
one of these linear transformations
is completely determined by where it takes i-hat and j-hat
but this time, each one of those basis vectors just lands on a number.
So when we record where they land as the columns of a matrix
each of those columns just has a single number.
This is a 1 x 2 matrix.
Let's walk through an example of what it means to apply one of these transformations to a
vector.

Let's say you have a linear transformation that takes i-hat to 1 and j-hat to -2.
To follow where a vector with coordinates, say, [4, 3] ends up,
think of breaking up this vector as 4 times i-hat + 3 times j-hat.
A consequence of linearity, is that after the transformation
the vector will be: 4 times the place where i-hat lands, 1,
plus 3 times the place where j-hat lands, -2.
which in this case implies that it lands on -2.

When you do this calculation purely numerically, it’s a matrix-vector multiplication.
Now, this numerical operation of multiplying a 1 by 2 matrix by a vector,
feels just like taking the dot product of two vectors.

Doesn't that 1 x 2 matrix just look like a vector that we tipped on its side?
In fact, we could say right now that there's a nice association between 1 x 2 matrices and 2D vectors,
defined by tilting the numerical representation of a vector on its side to get the associated matrix,
or to tip the matrix back up to get the associated vector.

Since we're just looking at numerical expressions right now,
going back and forth between vectors and 1 x 2 matrices might feel like a silly thing to do.

But this suggests something that's truly awesome from the geometric view:
there's some kind of connection between linear transformations that take vectors to numbers
and vectors themselves.

Let me show an example that clarifies the significance
and which just so happens to also answer the dot product puzzle from earlier.

Unlearn what you have learned
and imagine that you don't already know that the dot product relates to projection.
What I'm going to do here is take a copy of the number line
and place it diagonally and space somehow with the number 0 sitting at the origin.
Now think of the two-dimensional unit vector,
whose tips sit where the number 1 on the number line is.
I want to give that guy a name u-hat.
This little guy plays an important role in what's about to happen,
so just keep them in the back of your mind.
If we project 2D vectors straight onto this diagonal number line,
in effect, we've just defined a function that takes 2D vectors to numbers.
What's more, this function is actually linear
since it passes our visual test
that any line of evenly spaced dots remains evenly spaced once it lands on the number line.

Just to be clear,
even though I've embedded the number line in 2D space like this,
the output of the function are numbers, not 2D vectors.
You should think of a function that takes into coordinates and outputs a single coordinate.
But that vector u-hat is a two-dimensional vector living in the input space.

It's just situated in such a way that overlaps with the embedding of the number line.
With this projection, we just defined a linear transformation from 2D vectors to numbers,
so we're going to be able to find some kind of 1 x 2 matrix that describes that transformation.
To find that 1 x 2 matrix, let's zoom in on this diagonal number line setup
and think about where i-hat and j-hat each land,
since those landing spots are going to be the columns of the matrix.
This part's super cool, we can reason through it with a really elegant piece of symmetry:
since i-hat and u-hat are both unit vectors,
projecting i-hat onto the line passing through u-hat
looks totally symmetric to protecting u-hat onto the x-axis.
So when we asked what number does i-hat land on when it gets projected
the answer is going to be the same as whatever u-hat lands on when its projected onto the
x-axis but projecting u-hat onto the x-axis

just means taking the x-coordinate of u-hat.
So, by symmetry, the number where i-hat lands when it’s projected onto that diagonal number
line is going to be the x coordinate of u-hat.

Isn't that cool?
The reasoning is almost identical for the j-hat case.
Think about it for a moment.

For all the same reasons, the y-coordinate of u-hat
gives us the number where j-hat lands when it’s projected onto the number line copy.
Pause and ponder that for a moment; I just think that's really cool.

So the entries of the 1 x 2 matrix describing the projection transformation
are going to be the coordinates of u-hat.
And computing this projection transformation for arbitrary vectors in space,
which requires multiplying that matrix by those vectors,
is computationally identical to taking a dot product with u-hat.

This is why taking the dot product with a unit vector,
can be interpreted as projecting a vector onto the span of that unit vector and taking
the length.

So what about non-unit vectors?
For example,
let's say we take that unit vector u-hat,
but we “scale” it up by a factor of 3.
Numerically, each of its components gets multiplied by 3,
So looking at the matrix associated with that vector,
it takes i-hat and j-hat to 3 times the values where they landed before.

Since this is all linear, it implies more generally,
that the new matrix can be interpreted as projecting any vector onto the number line
copy and multiplying where it lands by 3.
This is why the dot product with a non-unit vector
can be interpreted as first projecting onto that vector
then scaling up the length of that projection by the length of the vector.
Take a moment to think about what happened here.

We had a linear transformation from 2D space to the number line,
which was not defined in terms of numerical vectors or numerical dot products.
It was just defined by projecting space onto a diagonal copy of the number line.
But because the transformation is linear,
it was necessarily described by some 1 x 2 matrix,
and since multiplying a 1 x 2 matrix by a 2D vector
is the same as turning that matrix on its side and taking a dot product,
this transformation was, inescapably, related to some 2D vector.

The lesson here, is that anytime you have one of these linear transformations
whose output space is the number line,
no matter how it was defined there's going to be some unique vector v
corresponding to that transformation,
in the sense that applying the transformation is the same thing as taking a dot product
with that vector.

To me, this is utterly beautiful.

It's an example of something in math called “duality”.
“Duality” shows up in many different ways and forms throughout math
and it's super tricky to actually define.

Loosely speaking, it refers to situations where you have a natural but surprising correspondence
between two types of mathematical thing.

For the linear algebra case that you just learned about,
you'd say that the “dual” of a vector is the linear transformation that it encodes.
And the dual of a linear transformation from space to one dimension,
is a certain vector in that space.

So, to sum up, on the surface, the dot product is a very useful geometric tool for understanding
projections and for testing whether or not vectors tend to point in the same direction.

And that's probably the most important thing for you to remember about the dot product,
but at deeper level, dotting two vectors together
is a way to translate one of them into the world of transformations:

again, numerically, this might feel like a silly point to emphasize,
it's just two computations that happen to look similar.
But the reason I find this so important,
is that throughout math, when you're dealing with a vector,
once you really get to know its personality
sometimes you realize that it's easier to understand it, not as an arrow in space,
but as the physical embodiment of a linear transformation.

It's as if the vector is really just a conceptual shorthand for certain transformation,
since it's easier for us to think about arrows and space
rather than moving all of that space to the number line.
