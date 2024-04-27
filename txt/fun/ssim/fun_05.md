## Information Management Insights
<a href="#information-management-insights"></a>
we need a chapter on how to construct ssimfiles -- i.e. find a set of items, do cardinality analysis
to do cardinality analysis, you find the sets whose product identifies an element of the set.
then you combine them with separators to form a structured key.
you look at the key and make sure that each component can indeed vary. if it can't vary, then it's not part of the key.
for instance, let's say you want to describe hosts in data centers. the primary key might be dc.hostname
but if the hostname is already globally unique, then it's wrong to include dc in the primary key, because you're not actually allowing the same hostname to appear in two dcs. in that case, dc becomes a function of hostname (i.e. field)

