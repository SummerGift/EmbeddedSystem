# 线性代数
## 线性方程组
## 行列式与克拉默法则
## 矩阵及其运算
## 神经网络中的矩阵/向量
## 矩阵的性质
## 矩阵与线性变换
## 线性变换的几何意义
## 特征值与特征向量
## NumPy 中矩阵的操作

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

## Abstract vector spaces

I'd like to revisit a deceptively simple question that I asked in the very first video of this
series, What are vectors?

Is a two-dimensional vector for example, fundamentally an arrow on a flat plane that we can describe
with co-ordinates for convenience? Or, is it fundamentally that pair of real numbers, 
which is just nicely visualised as an arrow on a flat plane?

Or are both of these just manifestations of something deeper?
On the one hand, defining vectors as primarily being a list of numbers feels clear-cut and
unambiguous.

It makes things like four-dimensional vectors or one hundred-dimensional vectors
sound like real, concrete ideas that you can actually work with.

When otherwise, an idea like four dimensions is just a vague, geometric notion that's difficult
to describe without waving your hands a bit.

But on the other hand, a common sensation for those who actually work with linear algebra,
especially as you get more fluent with changing your basis,
is that you're dealing with a space that exists independently from the co-ordinates that you
give it, and that co-ordinates are actually somewhat arbitrary,
depending on what you happen to choose as your basis vectors.

Core topics in linear algebra, like determinants and eigenvectors,
seem indifferent to your choice of co-ordinate systems.

The determinant tells you how much a transformation scales areas,
and eigenvectors are the ones that stay on their own span during a transformation.
But both of these properties are inherently spacial, 
and you can freely change your co-ordinate system without changing the underlying values of either one.

But, if vectors are fundamentally not lists of real numbers,
and if their underlying essence is something more spacial,
that just begs the question of what Mathematicians mean when they use a word like space or spacial.
To build up to where this is going, I'd actually like to spend the bulk of this video talking
about something which is neither an arrow, nor a list of numbers, but also has vector-ish qualities:
functions.

You see, there's a sense in which functions are actually just another type of vector.
In the same way that you can add two vectors together, there's also a sensible notion for
adding two functions, f and g, to get a new function, (f+g).

It's one of those thing where you kind of already know what it's gonna be, but actually
phrasing it is a mouthful.
The output of this new function at any given input, like -4, is the sum of the outputs
of f and g, when you evaluate them each at that same input, -4.
Or, more generally, the value of the sum function at any given input x is the sum of the values
of f(x) + g(x).

This is pretty similar to adding vectors co-ordinate by co-ordinate,
it's just that there are, in a sense, infinitely many co-ordinates to deal with.
Similarly, there's a sensible notion for scaling a function by a real number,
just scale all of the outputs by that number.
And again, this is analogous to scaling a vector co-ordinate by co-ordinate,
it just feels like there's infinitely many co-ordinates.

Now, given that the only thing vectors can really do is get added together or scaled,
it feels like we should be able to take the same useful constructs and problem solving
techniques of linear algebra, that were originally thought about in the context of arrows in space,
and apply them to functions as well.

For example, there's a perfectly reasonable notion of a linear transformation for functions,
something that takes in one function, and turns it into another.

One familiar example comes from calculus: the derivative.
It's something which transforms one function into another function.
Sometimes in this context, you'll hear these called operators instead of transformations,
but the meaning is the same.

A natural question you might want to ask, is what it means for a transformation of functions
to be linear.

The formal definition of linearity is relatively abstract and symbolically driven
compared to the way that I first talked about it in chapter 3 of this series,
but the reward of abstractness is that we'll get something general enough to apply to functions,
as well as arrows.

A transformation is linear if it satisfies two properties, commonly called additivity
and scaling. Additivity means that if you add two vectors, v and w, then apply a transformation to their
sum, you get the same result as if you added the transformed versions of v and w.

The scaling property is that when you scale a vector v by some number,
then apply the transformation, 
you get the same ultimate vector as if you scale the transformed version of v by that same amount.

The way you'll often hear this described is that linear transformations preserve the operations
of vector addition and scalar multiplication.

The idea of gridlines remaining parallel and evenly spaced is that I've talked about in
past videos is really just an illustration of what these two properties mean in the specific case of
2D points in space.

One of the most important consequences of these properties,
which makes matrix-vector multiplication possible, is that a linear transformation is completely
described by where it takes the basis vectors.

Since any vector can be expressed by scaling and adding the basis vectors in some way,
finding the transformed version of a vector comes down to scaling and adding the transformed
versions of the basis vectors in that same way.

As you'll see in a moment, this is as true for functions as it is for arrows.
For example, calculus students are always using the fact that the derivative is additive
and has the scaling property, even they haven't heard it phrased that way.

If you add two functions, then take the derivative, it's the same as first taking the derivative
of each one separately, then adding the result.
Similarly, if you scale a function, then take the derivative, it's the same as first taking
the derivative, then scaling the result.

To really drill in the parallel, let's see what it might look like to describe the derivative
with a matrix.

This will be a little tricky, since function spaces have a tendency to be infinite-dimensional,
but I think this exercise is actually quite satisfying.
Let's limit ourselves to polynomials, things like x^2 + 3x + 5 or 4x^7 - 5x^2.
Each of the polynomials in our space will only have finitely many terms,
but the full space is going to include polynomials with arbitrarily large degree.

The first thing we need to do is give co-ordinates to this space, which requires choosing a basis.
Since polynomials are already written down as the sum of scaled powers of the variable x,
it's pretty natural to just choose pure powers of x as the basis function.

In other words, our first basis function will be the constant function, b_0(x) = 1.
The second basis function will be b_1(x) = x, then b_2(x)=x^2, then b_3(x)=x^3, and so on.
The role that these basis functions serve will be similar to the roles of i-hat, j-hat
and k-hat in the world of vectors as arrows.

Since our polynomials can have arbitrarily large degree, this set of basis functions
is infinite. But that's okay, it just means that when we treat out polynomials as vectors,
they're going to have infinitely many co-ordinates.

A polynomial like x^2 + 3x + 5, for example, would be described with the co-ordiantes 5,
3, 1, then infinitely many zeros.
You'd read this as saying it's 5 times the first basis function, plus 3 times that second
basis function, plus 1 times the third basis function, and then none of the other basis functions
should be added from that point on.

The polynomial 4x^7 - 5x^2 would have the co-ordinates 0, 0, -5, 0, 0, 0, 0, 4, then
an infinite string of zeros.

In general, since every individual polynomial has only finitely many terms, it's co-ordinates
will be some finite string of numbers, with an infinite tail of zeros.

In this co-ordinate system, the derivative is described with an infinite matrix, that's
mostly full of zeors, but which has the positive integers counting down on this offset diagonal.

I'll talk about how you could find this matrix in just a moment, but the best way to get
a feel for it, is to just watch it in action.

Take the co-ordinates representing the polynomial x^3 + 5x^2 + 4x + 5,
then put those co-ordinates on the right of the matrix.
The only term which contributes to the first co-ordinate of the result is 1x4,
which means the constant term in the result will be 4.
This corresponds to the fact that the derivative of 4x is the constant 4.
The only term contributing to the second co-ordinate of the matrix-vector product is 2x5,
which means the coefficient in front of x in the derivative is 10.
That one corresponds to the derivative of 5x^2.
Similarly, the third co-ordinate in the matrix-vector product comes down to taking 3x1.
This one corresponds to the derivative of x^3 being 3x^2.
And after that, it'll be nothing but zeros.
What makes this possible is that the derivative is linear.
And, for those of you who like to pause and ponder, you could construct this matrix by
taking the derivative
of each basis function, and putting the co-ordinates of the results in each column.

So, surprisingly, matrix-vector multiplication and taking a derivative, which at first seem
like completely different animals, are both just really members of the same family.

In fact, most of the concepts I've talked about in this series with respect to vectors
as arrows in space,
things like the dot product or eigenvectors, have direct analogues in the world of functions.
Though sometimes they go by different names, things like 'inner product' or 'eigenfunction'.
So, back to the question of what is a vector.

The point I want to make here is that there are lots of vector-ish things in maths,
as long as you're dealing with a set of objects where there's a reasonable notion of scaling
and adding, whether that's a set of arrows in space, lists of numbers, functions or whatever other crazy
thing you choose to define, all of the tools developed in linear algebra regarding vectors,
linear transformations, and all that stuff, should be able to apply.

Take a moment to imagine yourself right now, as a mathematician developing the theory of
linear algebra.
You want all of the definitions and discoveries of your work to apply to all of the vector-ish
things in full generality, not just to one specific case.

These sets of vector-ish things, like arrows or lists of numbers or functions, are called
vector spaces, and what you as the mathematician might want to do is say,
"Hey everyone! I don't want to think about all the different types of crazy vector spaces
that you all might come up with, so what you do is establish a list of rules that vector addition
and scaling have to abide by.

These rules are called axioms, and in the modern theory of linear algebra, there are
8 axioms that any vector space must satisfy, if all of the theory and constructs that we've discovered
are going to apply.

I'll leave them on the screen here for anyone who wants to pause and ponder, but basically,
it's just a checklist, to make sure that the notions of vector addition and scalar multiplication
do the things that you'd expect them to do.

These axioms are not so much fundamental rules of nature, as they are an interface between
you, the mathematician discovering results, and other people who might want to apply those
results to new sorts of vectors spaces.

If, for example, someone defines some crazy type of vector space, like the set of all
pi creatures, with some definition of adding and scaling pi creatures, 
these axioms are like a checklist of things that they need to verify about their definitions, 
before they can start applying the results of linear algebra.

And you as the mathematician, never have to think about all the possible crazy vector
spaces people might define,
you just have to prove your results in terms of these axioms, so anyone who's definitions
satisfy those axioms can happily apply you results, even if you never thought about their situation.

As a consequence, you'd tend to phrase all of your results pretty abstractly, which is
to say, only in terms of these axioms,
rather than centring on a specific type of vector, like arrows in space, or functions.

For example, this is why just about every textbook you'll find will define linear transformations
in terms of additivity and scaling, rather than talking about gridlines remaining parallel
and evenly spaced,even though the latter is more intuitive, and at least in my view, 
more helpful for first time learners, even if it is specific to one situation.

So the mathematicians answer to "what are vectors?" is to just ignore the question.
In the modern theory, the form that vectors take doesn't really matter, arrows, lists
of numbers, functions, pi creatures, 
really it can be anything so long as there is some notion of adding and scaling vectors
that follows these rules.

It's like asking what the number 'three' really is.
Whenever it comes up concretely, it's in the context of some triplet of things, but in
maths, it's treated as an abstraction for all possible triplets of things, 
and lets you reason about all possible triplets, using a single idea.

Same goes with vectors, which have many embodiments, but maths abstracts them all into a single,
intangible notion of a vector space.

But, as anyone watching this series knows, I think it's better to begin reasoning about
vectors in a concrete, visualisable setting, like 2D space with arrows rooted at the origin.

But as you learn more linear algebra, know that these tools apply much more generally,
and that this is the underlying reason why textbooks and lectures tend to be phrased,
well, absractly.

So with that, folks, I think I'll call it an end to this essence of linear algebra series.
If you've watched and understood the videos, I really do believe that you have a solid
foundation in the underlying intuitions of linear algebra.

This is not the same thing as learning the full topic, of course, that's something that
can only really come from working through problems, 
but the learning you do moving forward can be substantially more efficient
if you have all the right intuitions in place.
So, have fun applying those intuitions, and best of luck with your future learning.
