; ModuleID = '../inputs/test.ll'
source_filename = "../inputs/test.ll"

define void @test_derived_iv(i32 %n) {
entry:
  br label %loop.header

loop.header:                                      ; preds = %loop.latch, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %i.next, %loop.latch ]
  %j.0 = phi i32 [ 100, %entry ], [ %j.next, %loop.latch ]
  %cmp = icmp slt i32 %i.0, %n
  br i1 %cmp, label %loop.body, label %loop.exit

loop.body:                                        ; preds = %loop.header
  %use = add i32 %j.0, 1
  br label %loop.latch

loop.latch:                                       ; preds = %loop.body
  %i.next = add nsw i32 %i.0, 1
  %j.next = add nsw i32 %j.0, 5
  br label %loop.header

loop.exit:                                        ; preds = %loop.header
  ret void
}
