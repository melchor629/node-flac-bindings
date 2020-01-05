module.exports = function(chai) {
    chai.assert.throwsAsync = async function(f, pattern) {
        try {
            await f();
        } catch(e) {
            if(pattern && !pattern.test(e.message)) {
                throw new Error(`Expected message '${e.message}' to match pattern ${pattern}`);
            }
            return;
        }
        throw new Error('Expected function to throw');
    };
};
