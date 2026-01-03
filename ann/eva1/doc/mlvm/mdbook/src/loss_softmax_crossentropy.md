# Softmax Cross-Entropy Loss

## The Problem

Given predictions \\( {\bf p} = [p_1, p_2, \ldots, p_n] \\) and targets \\( {\bf
y} = [y_1, y_2, \ldots, y_n] \\), the cross entropy loss is defined as
\\[
    {\rm Loss} = - \sum_{i} y_i \log p_i,
\\]
with gradient:
\\[
    {{\rm d} {\rm Loss} \over {\rm d} p_i} =
        - {{\rm d} y_i \log p_i \over {\rm d} p_i} = - {y_i \over p_i }.
\\]

However, this gradient is not numerical stable, as for any super small
\\(p_i\\), i.e., \\(p_i \to 0\\), the gradient is `NaN`.

## Solution

A common trick to solve this is calculating the gradient from loss \\({\rm
Loss}\\) w.r.t. the logits \\(o_i\\)  directly, where
\\[
    p_i = {\exp^{o_i} \over \sum_k \exp^{o_k}}.
\\]

The gradient, w.r.t. logits, is
\\[
    {{\rm d} {\rm Loss} \over {\rm d} o_i} =
         - \sum_k {{\rm d} {\rm Loss} \over {\rm d} p_k} {{\rm d} p_k \over {\rm d} o_i}.
\\]
where
\\[
    {{\rm d} {\rm Loss} \over {\rm d} p_k} = - {y_k \over p_k},
\\]
and
\\[
    {{\rm d} p_k \over {\rm d} o_i} =
        \cases { p_i (1-p_i), & if $k=i$; \cr
                 -p_k p_i & if $k\neq i$. \cr }
\\]

Substituting everything into the loss gradient, we get
\\[
    \eqalign{{{\rm d} {\rm Loss} \over {\rm d} o_i}
        & = (\dots) \cr
        & = \left( p_i \sum_k { y_k} \right) - y_i \cr
        & = p_i - y_i \cr
    }
\\]
where \\(\sum_k { y_k} = 1\\) given \\({\bf y}\\) is target probability.

## Numerical Stable For Softmax Cross-Entropy Loss

Now let's revisit the loss function with respect to the logits.
\\[
    \eqalign{
      {\rm Loss} & = - \sum_{i} y_i \log p_i, \cr
            & = - \sum_{i} \left(
                y_i \log {\exp^{o_i} \over \sum_k \exp^{o_k}}
              \right)\cr
    }
\\]
For any large logit \\(o_k\\), the \\(\exp\\) is \\(\inf\\), so unstable. The common trick
is to find the max item \\(o_{max}=\max_k o_k\\), and subtract it from all \\(o_k\\)s,
i.e.,
\\[
    \eqalign{
      {\rm Loss} & = - \sum_{i} \left(
                y_i \log {\exp^{o_i} \over \sum_k \exp^{o_k}}
                \right)\cr
            & = - \sum_{i} \left(
                y_i \log {\exp^{o_i-o_{max}} \over \sum_k \exp^{o_k-o_{max}}}
                \right)\cr
            & = -
                \left(
                   \sum_{i} y_i (o_i-o_{max})
                \right)
                + \log\left(
                    \sum_k \exp^{o_k-o_{max}}
                 \right)
              \cr
    }
\\]
