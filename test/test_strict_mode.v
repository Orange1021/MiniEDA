// Test module for strict mode validation
module test_strict(
    input in1,
    input in2,
    output out1
);

    wire n1;
    wire n1;  // Duplicate wire declaration

    AND2 and1(out1, in1, in2);  // Used undefined signal n2
    OR2 or1(n1, in1, undefined_signal);  // Used undefined signal

endmodule
