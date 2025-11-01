; ModuleID = 'test_derived_iv_nested.ll'
source_filename = "test_derived_iv_nested.c"

define void @test_derived_iv_nested(i32 %n) {
entry:
  br label %loop.header

; Outer loop
loop.header:
  %i.0 = phi i32 [ 0, %entry ], [ %i.next, %loop.latch ]
  %j.0 = phi i32 [ 100, %entry ], [ %j.next, %loop.latch ]
  %cmp = icmp slt i32 %i.0, %n
  br i1 %cmp, label %loop.body, label %loop.exit

loop.body:
  ; Use derived IV in dummy computation
  %use = add i32 %j.0, 1
  br label %loop.latch

loop.latch:
  %i.next = add nsw i32 %i.0, 1       ; canonical IV step
  %j.next = add nsw i32 %j.0, 5       ; derived IV step
  br label %loop.header

loop.exit:
  ret void
}
