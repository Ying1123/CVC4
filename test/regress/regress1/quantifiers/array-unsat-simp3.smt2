; COMMAND-LINE: --full-saturate-quant
; EXPECT: unsat
(set-logic AUFLIA)
(set-info :smt-lib-version 2.6)
(set-info :category "crafted")
(set-info :status unsat)
(declare-sort Index 0)
(declare-sort Element 0)
(declare-sort Array_uf 0)
(declare-fun store_uf (Array_uf Index Element) Array_uf)
(declare-fun select_uf (Array_uf Index) Element)
(declare-fun a1 () Array_uf)
(declare-fun e1 () Element)
(declare-fun e2 () Element)
(declare-fun i1 () Index)
(declare-fun i2 () Index)
(assert (forall ((?a Array_uf) (?i Index) (?e Element)) (= (select_uf (store_uf ?a ?i ?e) ?i) ?e)))
(assert (forall ((?a Array_uf) (?i Index) (?j Index) (?e Element)) (or (= ?i ?j)(= (select_uf (store_uf ?a ?i ?e) ?j) (select_uf ?a ?j)))))
(assert (forall ((?a Array_uf) (?b Array_uf)) (=> (forall ((?i Index)) (= (select_uf ?a ?i) (select_uf ?b ?i))) (= ?a ?b))))
(assert (not (= i1 i2)))
(assert (not (= (store_uf (store_uf a1 i1 e1) i2 e2) (store_uf (store_uf a1 i2 e2) i1 e1))))
(check-sat)
(exit)
