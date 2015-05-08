var gulp = require('gulp'),
    shell = require('gulp-shell');
 
gulp.task('texfiles', function () {
	return gulp.src('*.tex', {read: false})
    		.pipe(shell([
      		'pdflatex <%= file.path %>'
    		]));


});

gulp.task("watchTexFiles",function(){
	gulp.watch("*.tex", ['texfiles']);
});
gulp.task("default",["watchTexFiles"]);
