
library(ggplot2)

getBRC <- function(rect){
  x <- (rect$x + rect$w/2)
  y <- (rect$y - rect$h/2)
  return(list(x = x, y = y))
}

getTLC <- function(rect){
  x <- (rect$x - rect$w/2)
  y <- (rect$y + rect$h/2)
  return(list(x = x, y = y))
}

dist <- function(x1, y1, x2, y2){
  return(sqrt((x2 - x1)^2 + (y2 - y1)^2))
}

r1 <- list(x = 1, y = 3, w = 5, h = 3)
r2 <- list(x = 4, y = 3.5, w = 5, h = 3)

r1brc <- getBRC(r1)
r1tlc <- getTLC(r1)

r2brc <- getBRC(r2)
r2tlc <- getTLC(r2)

xmov <- r1brc$x - r2tlc$x
ymov <- r1tlc$y - r2brc$y
r3 <- list(x = 4 + xmov, y = 3.5 + ymov, w = 5, h = 3)

rectPlot <- ggplot() + 
  annotate('rect', xmin = r1$x - r1$w/2, ymin = r1$y - r1$h/2, xmax = r1$x + r1$w/2, ymax = r1$y + r1$h/2, color = 'grey', alpha = 0.2) +
  annotate('rect', xmin = r2$x - r2$w/2, ymin = r2$y - r2$h/2, xmax = r2$x + r2$w/2, ymax = r2$y + r2$h/2, color = 'grey', alpha = 0.2) +
  annotate('rect', xmin = r3$x - r3$w/2, ymin = r3$y - r3$h/2, xmax = r3$x + r3$w/2, ymax = r3$y + r3$h/2, color = 'blue', alpha = 0.2) +
  geom_text(aes(x = 1, y = 6, label = v), parse = TRUE) +
  geom_point(aes(x = c(r1$x, r2$x, r3$x), y = c(r1$y, r2$y, r3$y))) +
  scale_x_continuous(breaks = c(-10:10))

ggplot() +
  annotate('segment', x = 0, xend = 100, y = 0, yend = 0, color = 'red') +
  annotate('segment', x = 0, xend = 5, y = 0, yend = 5, color = 'blue') +
  annotate('segment', x = 0, xend = 0, y = 0, yend = 5, color = 'green')

print(paste('red', dist(0, 0, 100, 0)))
print(paste('blue', dist(0, 0, 100, 0)))


